#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>


int main (void) {
	uid_t *UID = malloc(sizeof(uid_t));
	gid_t *GID = malloc(sizeof(gid_t));
	int *UIDFlag = malloc(sizeof(int));
    	int *GIDFlag = malloc(sizeof(int));

	int eventID = syscall(181);
	
	printf("created eventID = %d \n", eventID);
	
	int ret = syscall(214, eventID, UID, GID, UIDFlag, GIDFlag);
	
	printf("UIDFlag: %d\n",*UIDFlag);
	printf("GIDFlag: %d\n",*GIDFlag);

	ret = syscall(211, eventID, 1, 0);

	printf("Root can change mode of event: %d\n",eventID);
	printf("New UIDFlag: %d\n",*UIDFlag);
	printf("New GIDFlag: %d\n",*GIDFlag);

	ret = syscall(214, eventID, UID, GID, UIDFlag, GIDFlag);
	
	printf("EVENTSTAT: UID: %d\n",*UID);
	printf("GID: %d\n",*GID);
	printf("UIDFlag: %d\n", *UIDFlag);
	printf("GIDFlag: %d\n", *GIDFlag);	

	return 0;
}

