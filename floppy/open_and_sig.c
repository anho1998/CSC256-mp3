#include <unistd.h>
#include <doevent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
  long res;
  int eventID = doeventopen();
  if (eventID == -1){
    printf("Couldn't open!\n");
    exit(1);
  }
  char ar[25];
  memset(ar, 0, sizeof(char)*25);

  printf("event id is %d\n", eventID);

  scanf("a", ar);

  res = doeventclose(eventID);
  printf("closed? %d\n", res);
}
