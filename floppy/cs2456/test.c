#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>


int main (void) {
	int a, b;
	/* Print a friendly message */
	printf ("Driving to kernel\n");

	/* Call our new system call */
	a = syscall (181);
	b = syscall (181);

	printf("Rising to user\n");

	printf("a:%d, b:%d\n",a,b);
	/* Exit the program */
	return 0;
}

