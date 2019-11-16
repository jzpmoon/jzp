#include <stdio.h>
#include "ustack_int.h"

ustackint stack = {NULL,NULL,0,0,-1};

int main(){
  int i = 0;
  int j = 0;
  while(i < 10){
    ustack_pushint(&stack,i);
    i++;
  }
  
  while(j < 10){
    int val;
    ustack_popint(&stack,&val);
    printf("val:%d\n",val);
    j++;
  }
  
  return 0;
}
