#include "/home/cranberry/Projects/clearScreen/clearScreenLinux.h"
#include "tomatoInLine.h"

#include <asm-generic/errno.h>
#include <bits/getopt_core.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PIPE_NAME "/tmp/tomato_pipe"
#define TEST_MODE 99

int wait = 0;

void clockTimer(int time) {
  while (time) {
    if (wait)
      continue;
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

void readFromTomatoPipe() {
  int fd = open(PIPE_NAME, O_NONBLOCK | O_RDONLY, 0664);
  if (fd == -1){
      perror("can't open to read pipe");
  }
  int number = 0;
  int rdn = 0;
  while (1) {
    rdn = read(fd, &number, sizeof(int));
    if (rdn) {
      wait = number;
    }
  }
}

void writeToTomatoPipe(int number) {
  int fd = open(PIPE_NAME, O_WRONLY | O_NONBLOCK);
  if (fd) {
    int f = write(fd, &number, sizeof(int));
  } else {
    perror("can't write to pipe");
  }
}

// return pipe fd
void initPipe() {

  int fd = open(PIPE_NAME, O_RDONLY | O_NONBLOCK);
  if (fd == -1) {
    switch (errno) {
    case ENOENT:
      fd = mkfifo(PIPE_NAME, 0666);
      if (fd == -1) {
        perror("error, cann't create a tomato pipe");
        exit(EXIT_FAILURE);
      }
      break;
    default:
      perror("can't open tomato pipe");
      exit(EXIT_FAILURE);
      break;
    }
  }
  close(fd);
}

int main(int argc, char **argv) {

  int opt = 0, fd = 0;

  while ((opt = getopt(argc, argv, "ic:h")) != -1) {

    switch (opt) {
    case 'h':
      print_help();
      break;
    case 'i':
      initTimer();
      initPipe();
      readFromTomatoPipe();
      close(fd);
      pthread_exit(NULL);
      break;
    case 'c':
      initPipe();
      writeToTomatoPipe(atoi(optarg));
      break;
    }
    exit(EXIT_SUCCESS);
  }
}
