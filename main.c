#include "/home/cranberry/Projects/clearScreen/clearScreenLinux.h"
#include "tomatoInLine.h"

#include <asm-generic/errno.h>
#include <bits/getopt_core.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PIPE_NAME "/tmp/tomato_pipe"
#define TEST_MODE 99

static int app_mode = 0;
static int state = 0;

pthread_mutex_t stop;
pthread_mutex_t state_mu;

void drop_app(int sig) {
  if (app_mode == MODE_CLOCK) {
    unlink(PIPE_NAME);
    exit(EXIT_SUCCESS);
  }
}

void clockTimer(int time) {
  int n_time = time;
  while (n_time) {
     clearScreen();
     printf("%02d : %02d\n", n_time / 60, n_time % 60);
        //printf("%d\n",state); 
    switch (state) {
    case S_STOP:
      puts("rich lock");
      pthread_mutex_lock(&stop);
      break;
    case S_PROGRESS:
      sleep(1);
      n_time--;
      break;
    case S_RESTART:
      n_time = time;
      state = S_PROGRESS;
      break;
    case S_SKIP:
      state = S_PROGRESS;
         return;
      break;
    case S_KILL:
      kill(getpid(), SIGINT);
      break;
    }
  }
}

void *init_mode(void *arg) {
  int mode = (int)(intptr_t)arg;
  int work_time = 0;
  int rest_time = 0;
  state = S_PROGRESS;

  pthread_mutex_init(&stop, NULL);
  pthread_mutex_init(&state_mu,  NULL);

  switch (mode) {
  case 1:
    work_time = 25 * 60;
    rest_time = 5 * 60;
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

void init_timer() {

  pthread_t clock;
  pthread_create(&clock, NULL, init_mode, (void *)(intptr_t)1);
}

void read_from_tomato_pipe() {
  int fd = open(PIPE_NAME, O_NONBLOCK | O_RDONLY, 0664);
  if (fd == -1) {
    perror("can't open to read pipe");
  }
  char cmd;
  int rd;
  while (1) {
    rd = read(fd, &cmd, sizeof(char));
    if (rd){
    switch (cmd) {
    case 's':
      state = S_STOP;
      break;
    case 'c':
      state = S_PROGRESS;
      pthread_mutex_unlock(&stop);
      break;
    case 'k':
      state = S_KILL;
      pthread_mutex_unlock(&stop);
      break;
    case 'r':
      state = S_RESTART;
      pthread_mutex_unlock(&stop);
      break;
    case 'p':
      state = S_SKIP;
      pthread_mutex_unlock(&stop);
      break;
    }
    }
  }
  close(fd);
}

void write_to_tomato_pipe(char cmd) {
  int fd = open(PIPE_NAME, O_WRONLY);
  if (fd) {
    int f = write(fd, &cmd, sizeof(char));
  } else {
    perror("can't write to pipe");
  }
  close(fd);
}

void init_pipe() {

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
  signal(SIGINT, drop_app);
  int opt = 0, fd = 0;

  while ((opt = getopt(argc, argv, "ic:h")) != -1) {

    switch (opt) {
    case 'h':
      print_help();
      break;
    case 'i':
      app_mode = MODE_CLOCK;
      init_pipe();
      init_timer();
      read_from_tomato_pipe();
      close(fd);
      pthread_exit(NULL);
      break;
    case 'c':
      if (strlen(optarg) == 1) {
        app_mode = MODE_CONTROL;
        write_to_tomato_pipe(optarg[0]);
      } else {
        puts("wrong control input");
      }
      break;
    }
    exit(EXIT_SUCCESS);
  }
}
