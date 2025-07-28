#pragma once
#include <stdio.h>


#define S_PROGRESS 1
#define S_STOP 2
#define S_KILL 3
#define S_RESTART 4
#define S_SKIP 5


#define MODE_CLOCK 1
#define MODE_CONTROL 2

void print_help(){
   puts("toamto have two modes");
   puts("-i {n} where n is number of mode");
   puts("-c to controll");
   puts("   -c s to stop");
   puts("   -c c to continue");
   puts("   -c k to kill app tomato timer");
   puts("   -c r to restart timer");
   puts("   -c p to skip curent timer");
}



