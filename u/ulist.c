#include <stddef.h>
#include <malloc.h>
#include "uassert.h"
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
