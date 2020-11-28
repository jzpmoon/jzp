#include <stddef.h>
#include <stdio.h>
#include "uerror.h"
#include "ulist.h"

int test1(){
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
  return 0;
}

int test2(){
  char* arr[]={"1","2","3","4","5"};
  int i=0;
  ulist_ucharp* list=ulist_ucharp_new();
  ucursor cursor;
  while(i<5){
    ulist_ucharp_append(list,arr[i]);
    i++;
  }
  ulog("iterate");
  (*list->iterate)(&cursor);
  while(1){
    ucharp* str = (*list->next)((uset*)list,&cursor);
    if(str == NULL){
      break;
    }
    printf("%s\n",*str);
  }
  return 0;
}

int main(){
  test2();
  return 0;
}
