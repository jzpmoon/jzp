#include <stdio.h>
#include "ustack_int.h"

ustack_int stack = {NULL,NULL,0,0,-1};

int main(){
  int i = 0;
  int j = 0;
  while(i < 10){
    ustack_push_int(&stack,i);
    i++;
  }
  
  while(j < 10){
    int val;
    ustack_pop_int(&stack,&val);
    printf("val:%d\n",val);
    j++;
  }
  
  return 0;
}
