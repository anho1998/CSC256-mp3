#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>


int main (void) {
	int c;
	/* Print a friendly message */
	printf ("Driving to kernel\n");

	/* Call our new system call */
	c = syscall(211, 0, 1, 0);

	printf("Rising to user\n");

	printf("c:%d\n",c);
	/* Exit the program */
	return 0;
}

