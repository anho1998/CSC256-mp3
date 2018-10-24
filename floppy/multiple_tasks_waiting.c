#include <doevent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv){
  if (argc < 2){
    printf("Arguments not correct...\n");
    exit(1);
  }
  printf("Process %d is about to wait!\n", getpid());
  long res = doeventwait(atoi(argv[1]));
  if (res == -1){
    printf("Something went wrong in waiting\n");
  } else {
    printf("Process %d is done waiting!\n", getpid());
  }
}
