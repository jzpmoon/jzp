#include <stddef.h>
#include "ualloc.h"
#include "ulist_tpl.h"

#define ulist_append_def_tpl(t)				\
  int ulist_##t##_append(ulist_##t* list,t value){	\
    ulsnd_##t* header=list->header;			\
    ulsnd_##t* footer=NULL;				\
    ulsnd_##t* node=NULL;				\
    unew(node,sizeof(ulist_##t),return -1;)		\
      if(header==NULL){					\
	header=footer=node;				\
      }else{						\
	footer=header->prev;				\
      }							\
    header->prev=node;					\
    footer->next=node;					\
    node->prev=footer;					\
    node->next=header;					\
    node->value=value;					\
    list->len++;					\
    return 0;						\
  }

#define ulist_def_tpl(t)			\
  ulist_append_def_tpl(t)
