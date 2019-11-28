#include <stdio.h>
#include "ustack.h"

ustack_int stack;

int main(){
  int i = 0;
  int j = 0;
  ustack_init(int,&stack,-1,-1);
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
