#include <stdio.h>
#include <stdlib.h>
#include <doevent.h>

#include <linux/unistd.h>
#include <sys/syscall.h>

int main () {
  // I'm going to make an event and wait on it.
  // Signal will be provided later by another process

  long res;

  int event = (int)doeventopen();
  if (event == -1){
    printf("event is invalid\n");
    exit(1);
  }
  res = doeventsig(event);
  if (res == -1){
    printf("Something went horribly wrong :(\n");
  }
  res = doeventclose(event);
  if (res == -1){
    printf("Couldn't close?\n");
  }
}
