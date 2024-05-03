## Working Set Size Estimation via Linux Kernel

### CS695: Virtualization and Cloud Computing

### Instructions to reproduce experiment

1. Compile `g++ test1/2.cpp -o test1/2`
2. Execute `sudo ./test1/2 &`
3. To reproduce first approach, run `sudo ./wss.pl [pid] [polling interval]`
4. To reproduce second approach, compile `gcc wss1.c` and run `sudo ./a.out [pid] [polling interval] [number of time steps to cummulate]`
5. To generate plots, redirect output to appropriate filenames as present in results folder and then run `script.sh [test1/test2]` and then run plotgen.py.


#### References: <br>
1. https://www.kernel.org/doc/Documentation/vm/idle_page_tracking.txt
2. https://www.kernel.org/doc/Documentation/vm/pagemap.txt
3. https://www.kernel.org/doc/gorman/html/understand/understand006.html
4. https://man7.org/linux/man-pages/man2/getpagesize.2.html
5. https://lwn.net/Articles/459269/
6. https://lwn.net/Articles/642202/
7. https://kernelnewbies.org/Linux_4.3#Introduce_idle_page_tracking.2C_a_more_precise_way_to_track_the_memory_being_used_by_applications
8. https://learn.microsoft.com/en-us/windows-hardware/test/wpt/wpa-reference-set
9. https://lkml.iu.edu/hypermail/linux/kernel/0702.1/0628.html
10. https://www.networkworld.com/article/831128/software-kernel-space-how-much-memory-am-i-really-using.html

##### Linux Kernel Source
1. https://elixir.bootlin.com/linux/v6.5/source/mm/workingset.c
2. https://elixir.bootlin.com/linux/v6.5/source/mm/page_idle.c
3. https://elixir.bootlin.com/linux/v6.5/source/mm/page_idle.h
4. https://elixir.bootlin.com/linux/v6.5/source/include/linux/page-flags.h
5. https://elixir.bootlin.com/linux/v6.5/source/include/linux/page_ext.h
6. https://elixir.bootlin.com/linux/v6.5/source/include/linux/pagemap.h
7. https://elixir.bootlin.com/linux/v6.5/source/include/linux/mm.h

#### Further exploration:
1. https://hal.science/hal-03622708/document
2. https://www.usenix.org/legacy/event/atc11/tech/final_files/Zhao.pdf
3. https://arxiv.org/pdf/1902.11028.pdf
