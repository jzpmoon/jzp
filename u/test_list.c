#include <stddef.h>
#include <stdio.h>
#include "ulist.h"

int main(){
  char* arr[]={"1","2","3","4","5"};
  int i=0,j=0;
  ulist* list=NULL;
  ulist* node=NULL;
  while(i<5){
    list=ulist_append(list,arr[i]);
    i++;
  }
  node=list;
  while(j<10){
    printf("%s\n",(char*)node->value);
    node=node->next;
    j++;
  }
}
