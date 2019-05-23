#include <stddef.h>
#include "ualloc.h"
#include "ulist.h"

void* ulist_append(ulist* list,void* value){
  ulist* header=list;
  ulist* footer=NULL;
  ulist* node=NULL;
  unew(node,sizeof(ulist),return NULL;)
  if(header==NULL){
    header=footer=node;
  }else{
    footer=header->prev;
  }
  header->prev=node;
  footer->next=node;
  node->prev=footer;
  node->next=header;
  node->value=value;
  return header;
}

int ulist_length(ulist* list){
  ulist* header=list;
  ulist* node=list;
  int length=0;
  while(node!=header){
    node=node->next;
    length++;
  }
  return length;
}
