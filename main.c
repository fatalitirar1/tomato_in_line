#include "/home/cranberry/Projects/clearScreen/clearScreenLinux.h"
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void *clockTimer(void *arg) {
  while (1) {
    printf("do impr\n");
    sleep(1);
    clearScreen();
  }
}

void initTimer() {

  pthread_t clock;
  pthread_create(&clock, NULL, clockTimer, NULL);
}

int main(int argc, char **argv) {

  int tomato_data;
  int fd = open("tomato_data", O_RDONLY);

  if (fd == -1) 
  {

    switch (errno)
    {
    
    case ENOENT:

      tomato_data = mkfifo("tomato_data", 0666);

      if (tomato_data == -1) {
        perror("error while create an fifo chanel");
      }
      break;

    default:

      perror("cannot open fifo chanel");
      break;
    }

    unlink("tomato_data");
    pthread_exit(NULL);
    exit(EXIT_SUCCESS);
  }
}
