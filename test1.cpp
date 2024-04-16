#include <iostream>
#include <climits>
#include <unistd.h>
using namespace std;

int main(){
	int a[10000];
	int j=0, n=1000;
	for(int i=0; i<=INT_MAX; i++){
		int *new_int_ptr1 = new int(i);
		int *new_int_ptr2 = new int(i);
		int *new_int_ptr3 = new int(i);
		int *new_int_ptr4 = new int(i);
		a[(i+1)%10000]=*new_int_ptr1;
		a[(i+2)%10000]=*new_int_ptr2;
		a[(i+3)%10000]=*new_int_ptr3;
		a[(i+4)%10000]=*new_int_ptr4;
		sleep(0.0001);
		if(i==INT_MAX && j!=n) {i=0; j++;}
		else if(i==INT_MAX && j==n) break;
	}
	return 0;
}
