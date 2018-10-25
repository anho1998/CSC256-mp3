#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>


int main (void) {
	int events[20], info[20];
	int i, active, n = 10;
	/* Print a friendly message */
	printf ("Driving to kernel\n");

	/* Call our new system call */
	for (i=0; i<n; i++){	
		events[i] = syscall (181);		
	}
	printf("Rising to user\n");
	active = syscall(185, n+10, info);	
	for (i=0; i<active-1; i++){
		printf("event ID: %d, info: %d\n",events[i], info[i]);
		if (events[i] == -1)
			printf("i=%d ERROR!!!\n",i);	
	}
	/* Exit the program */
	return 0;
}

