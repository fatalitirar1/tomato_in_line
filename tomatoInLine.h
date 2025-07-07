#pragma once
#include <stdio.h>

void print_help(){
   puts("toamto have two modes");
   puts("-i {n} where n is number of mode");
   puts("-c to controll");
}

typedef struct Tomato{
   int state;
   int mode;
}Tomato;
