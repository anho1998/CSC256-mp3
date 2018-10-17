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
	if(eventID == -1){
		printf("Error in doeventopen()\n");
		return 0;
	}
	printf("Newly created event with eventID = %d \n", eventID);
	
	int ret = syscall(214, eventID, UID, GID, UIDFlag, GIDFlag);
	if(ret == -1){
		printf("Error in doeventstat()\n");
		return 0;
	}
	printf("Event effective UIDFlag: %d\n",*UIDFlag);
	printf("Event effective GID: %d\n",*GIDFlag);

    	printf("Process is root can change everthing.\n");
	ret = syscall(211, eventID, 0, 0);
	if(ret == -1){
		printf("Fail to doeventchown()\n");
	}
	else{
		printf("Successfully change mode of event: %d\n",eventID);
		printf("Event new effective UID: %d\n",*UID);
		printf("Event new effective GID: %d\n",*GID);
        printf("Correct\n");
	}

	ret = syscall(214, eventID, UID, GID, UIDFlag, GIDFlag);
	if(ret == -1){
		printf("Error in doeventstat()\n");
		return 0;
	}
	printf("Event new effective UID: %d\n",*UID);
	printf("Event new effective GID: %d\n",*GID);
	printf("Event new effective UIDFlag: %d\n", *UIDFlag);
	printf("Event new effective GIDFlag: %d\n", *GIDFlag);	

	return 0;
}

