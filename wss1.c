/*
 * wss1.c	Estimate the working set size (WSS) for a process on Linux.
 	Suited for small processes.
 *
 * This version walks page structures one by one, and is suited for small processes. 
 * See wss2.c which snapshots page
 * data, and can be over 50x faster for large processes, although slower for
 * small processes. There is also wss.pl, which can be over 500x faster,
 * however, uses the referenced page flag.
 * COMPILE: gcc -o wss-v1 wss-v1.c
 *
 * REQUIREMENTS: Linux 6.5
 *
 * USAGE: wss PID duration(s)
 *
 * COLUMNS:
 *	- Est(s):  Estimated WSS measurement duration: this accounts for delays
 *	           with setting and reading pagemap data, which inflates the
 *	           intended sleep duration.
 *	- Ref(MB): Referenced (Mbytes) during the specified duration.
 *	           This is the working set size metric.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

// see Documentation/vm/pagemap.txt:
#define PFN_MASK		(~(0x1ffLLU << 55))

#define PATHSIZE		128
#define LINESIZE		256
#define PAGEMAP_CHUNK_SIZE	8

// from mm/page_idle.c:
#ifndef BITMAP_CHUNK_SIZE
#define BITMAP_CHUNK_SIZE	8
#endif

#ifndef PAGE_OFFSET
#define PAGE_OFFSET		0xffff880000000000LLU
#endif

enum {
	READIDLE = 0,
	SETIDLE
};

// globals
int g_debug = 1;		// 1 == some, 2 == verbose
int g_activepages = 0;
int g_walkedpages = 0;

int mapidle(pid_t pid, unsigned long long mapstart, unsigned long long mapend, int action)
{
	char pagepath[PATHSIZE];
	char *idlepath = "/sys/kernel/mm/page_idle/bitmap";
	int idlefd, pagefd;
	char *line;
	int pagesize = getpagesize();
	unsigned long long p, pagemapp, pfn, idlemapp, idlebits;
	int flags;
	int err = 0;

	// open pagemap for virtual to PFN translation
	if (sprintf(pagepath, "/proc/%d/pagemap", pid) < 0) {
		printf("Can't allocate memory. Exiting.");
		exit(1);
	}
	if ((pagefd = open(pagepath, O_RDONLY)) < 0) {
		perror("Can't read pagemap file");
		return 2;
	}

	// open idlemap for WSS estimation
	flags = O_RDONLY;
	if (action == SETIDLE)
		flags = O_WRONLY;
	if ((idlefd = open(idlepath, flags)) < 0) {
		perror("Can't read idlemap file");
		close(pagefd);
		return 2;
	}

	// walk pagemap to get PFN, then operate on PFN from idlemap
	for (p = mapstart; p < mapend; p += pagesize) {
		pagemapp = PAGEMAP_CHUNK_SIZE * p / pagesize;

		/*
		 * The following involves a lot of syscalls for setting and
		 * reading bits. This is why this program is slow. This should
		 * be optimized to read by chunks. Or to use mmap, however, I
		 * don't think the kernel files involved support an mmap
		 * interface. Does Linux provide a
		 * /proc/PID/clear_idle interface with an entry in
		 * /proc/PID/smaps? This would make this much faster.
		 */

		// convert virtual address p to physical PFN
		if (lseek(pagefd, pagemapp, SEEK_SET) < 0) {
			printf("Can't seek pagemap file\n");
			goto out;
		}
		if (read(pagefd, &pfn, sizeof (pfn)) < 0) {
			printf("Can't read pagemap file\n");
			goto out;
		}
		pfn = pfn & PFN_MASK;
		if (pfn == 0)
			continue;

		// locate idle map byte
		idlemapp = (pfn / 64) * BITMAP_CHUNK_SIZE;
		if (lseek(idlefd, idlemapp, SEEK_SET) < 0) {
			printf("Can't seek idlemap file\n");
			goto out;
		}
		if (g_debug > 1) {
			printf("%s: p %llx pfn %llx idlebits %llx\n",
			    action == READIDLE ? "R" : "W", p, pfn, idlebits);
		}

		/*This is based on the description in
		 * Documentation/vm/idle_page_tracking.txt.
		 */

		// read idle bit
		if (action == READIDLE) {
			if (read(idlefd, &idlebits, sizeof (idlebits)) <= 0) {
				perror("Can't read idlemap file");
				goto out;
			}
			if (!(idlebits & (1ULL << (pfn % 64)))) {
				// TODO: optimize
				g_activepages++;
			}
			g_walkedpages++;

		// set idle bit
		} else /* SETIDLE */ {
			idlebits = ~0ULL;
			if (write(idlefd, &idlebits, sizeof (idlebits)) <= 0) {
				perror("Can't write idlemap file");
				goto out;
			}
		}
	}

out:
	close(pagefd);
	close(idlefd);

	return err;
}

int walkmaps(pid_t pid, int action)
{
	FILE *mapsfile;
	char mapspath[PATHSIZE];
	char line[LINESIZE];
	unsigned long long mapstart, mapend;

	// read virtual mappings
	if (sprintf(mapspath, "/proc/%d/maps", pid) < 0) {
		printf("Can't allocate memory. Exiting.");
		exit(1);
	}
	if ((mapsfile = fopen(mapspath, "r")) == NULL) {
		perror("Can't read maps file");
		exit(2);
	}

	while (fgets(line, sizeof (line), mapsfile) != NULL) {
		sscanf(line, "%llx-%llx", &mapstart, &mapend);
		if (g_debug>1)
			printf("MAP %llx-%llx\n", mapstart, mapend);
		if (mapstart > PAGE_OFFSET)
			continue;	// page idle tracking is user mem only
		if (mapidle(pid, mapstart, mapend, action)) {
			printf("Error setting map %llx-%llx. Exiting.\n",
			    mapstart, mapend);
		}
	}

	fclose(mapsfile);

	return 0;
}

int main(int argc, char *argv[])
{
	pid_t pid;
	double duration, mbytes;
	static struct timeval ts1, ts2;
	unsigned long long set_us, read_us, dur_us, slp_us, est_us;


	// options
	if (argc < 3) {
		printf("USAGE: wss PID duration(s) [num_loops]\n");
		exit(0);
	}	
	pid = atoi(argv[1]);
	duration = atof(argv[2]);
	if (duration < 0.01) {
		printf("Interval too short. Exiting.\n");
		return 1;
	}
	int num_loops = 1;
	if (argc >= 4) {
		num_loops = atoi(argv[3]);
	}
	struct timeval ts3[num_loops], ts4[num_loops];
	int active_till_now[num_loops];

	printf("Watching PID %d page references during %.2f * %d seconds...\n",
	    pid, duration, num_loops);

	// set idle flags
	gettimeofday(&ts1, NULL);
	walkmaps(pid, SETIDLE);

	// sleep
	gettimeofday(&ts2, NULL);
	for (int i = 0; i < num_loops; i++){
		usleep((int)(duration * 1000000));
		gettimeofday(ts3 + i, NULL);

		// read idle flags
		walkmaps(pid, READIDLE);
		gettimeofday(ts4 + i, NULL);
		active_till_now[i] = g_activepages;
	}

	// calculate times
	set_us = 1000000 * (ts2.tv_sec - ts1.tv_sec) +
	    (ts2.tv_usec - ts1.tv_usec);
	if (g_debug) {
		printf("set time  : %.3f s\n", (double)set_us / 1000000);
	}
	printf("%-7s %10s\n", "Est(s)", "Ref(MB)");
	for (int i = 0; i < num_loops; i++){
		slp_us = 1000000 * (ts3[i].tv_sec - ts2.tv_sec) +
			(ts3[i].tv_usec - ts2.tv_usec);
		read_us = 1000000 * (ts4[i].tv_sec - ts3[i].tv_sec) +
			(ts4[i].tv_usec - ts3[i].tv_usec);
		dur_us = 1000000 * (ts4[i].tv_sec - ts1.tv_sec) +
			(ts4[i].tv_usec - ts1.tv_usec);
		est_us = dur_us - (read_us / 2);
		est_us = i == 0 ? est_us - (set_us / 2) : est_us;
		if (g_debug) {
			printf("sleep time: %.3f s\n", (double)slp_us / 1000000);
			printf("read time : %.3f s\n", (double)read_us / 1000000);
			printf("dur time  : %.3f s\n", (double)dur_us / 1000000);
			// assume getpagesize() sized pages:
			printf("referenced: %d pages, %d Kbytes\n", active_till_now[i],
				active_till_now[i] * getpagesize());
			printf("walked    : %d pages, %d Kbytes\n", g_walkedpages,
				g_walkedpages * getpagesize());
		}
		// assume getpagesize() sized pages:
		mbytes = (active_till_now[i] * getpagesize()) / (1024 * 1024);
		printf("%-7.3f %10.2f\n", (double)est_us / 1000000, mbytes);
	}

	return 0;
}
