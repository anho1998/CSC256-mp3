#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>


int main (void) {
	int child, ret;
	int eventID = syscall(181);

	if(eventID == -1){
		printf("ERROR OPEN!!!\n");
        	return 0;
	}
	
	if(fork() == 0){ // child process
        	child = syscall(183, eventID);
        	if(child == -1){
			printf("ERROR WAIT!!!. \n");
		} 
        	return 0;
    	}
	else{
        	sleep(1);
        	ret = syscall(182, eventID);
        if(ret == -1){
            printf("ERROR CLOSE \n");
            return 0;
        }
        printf("Close #%d, unblock %d\n", eventID, ret);
    	}
	return 0;
}
