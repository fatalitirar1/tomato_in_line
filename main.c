#include "/home/cranberry/Projects/clearScreen/clearScreenLinux.h"
#include "tomatoInLine.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>


#define TEST_MODE 99

void clockTimer(int time) {
  while (time) {
    printf("%d : %d\n", time / 60, time % 60);
    sleep(1);
    clearScreen();
    time--;
  }
}

void *initMode(void *arg) {
  int mode = (int)(intptr_t)arg;
  int work_time = 0;
  int rest_time = 0;

  switch (mode) {
  case 1:
    work_time = 25 * 60;
    rest_time = 25 * 60;
    break;
  case TEST_MODE:
    work_time = 5;
    rest_time = 5;
    break;
  default:
    printf("corrapted mode %d \n", mode);
    break;
  }
  while (1) {
    clockTimer(work_time);
    clockTimer(rest_time);
  }
  return 0;
}

void initTimer() {

  pthread_t clock;
  pthread_create(&clock, NULL, initMode, (void *)(intptr_t)TEST_MODE);
}

int main(int argc, char **argv) {

  if (argc == 1) {
    print_help();
  } else {
    if (strcasecmp(argv[1], "-i")) {
      initTimer();
      pthread_exit(NULL);
    } else if (strcasecmp(argv[1], "-c")) {
      puts(":^)");
    }
  }
  exit(EXIT_SUCCESS);
}
