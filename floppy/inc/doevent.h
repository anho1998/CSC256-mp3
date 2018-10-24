#ifndef DOEVENT_H
#define DOEVENT_H
#define _GNU_SOURCE

#include <unistd.h>
#include <sys/syscall.h>

long doeventopen(){
  return syscall(181);
}

long doeventclose(int eventID){
  return syscall(182, eventID);
}

long doeventwait(int eventID){
  return syscall(183, eventID);
}

long doeventsig(int eventID){
  return syscall(184, eventID);
}

long doeventinfo(int num, int *eventIDs){
  return syscall(185, num, eventIDs);
}

long doeventchown(int eventID, uid_t UID, gid_t GID){
  return syscall(205, eventID, UID, GID);
}

long doeventchmod (int eventID, int UIDFlag, int GIDFlag){
  return syscall(211, eventID, UIDFlag, GIDFlag);
}

long doeventstat (int eventID, uid_t *UID, gid_t GID, int *UIDFlag, int *GIDFlag){
  return syscall(214, eventID, UID, GID, UIDFlag, GIDFlag);
}

#endif
