#include <iostream>
#include <climits>
#include <unistd.h>
#include <random>
#include <cstdlib>
using namespace std;

int main(){
    int arrsize=10;
    int a[1024*arrsize];
    sleep(10);
    for(int i=0; i<arrsize; i++){
        system("sudo sync > /dev/null 2>&1");
        system("sudo sysctl -w vm.drop_caches=3 > /dev/null 2>&1");
        a[i*1024]=i;
        // sleep(2);
        usleep(10000);
        // cout << "i: " << i << endl;
        if(i==arrsize-1) i=0;
    }
	return 0;
}
