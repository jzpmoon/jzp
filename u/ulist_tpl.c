#include <stddef.h>
#include "ualloc.h"
#include "ulist_tpl.h"

#define ulist_cursor_tpl(t)					\
  static void ulist_##t##_cursor_init(ucursor* cursor){		\
    ulist_##t##_cursor* c = (ulist_##t##_cursor*)cursor;	\
    c->next = NULL;						\
  }

#define ulist_cursor_next_tpl(t)					\
  static void* ulist_##t##_cursor_next(uset* set,ucursor* cursor){	\
    ulist_##t* list = (ulist_##t*)set;					\
    ulist_##t##_cursor* c = (ulist_##t##_cursor*)cursor;		\
    ulsnd_##t* next;							\
    if(!list->header){							\
      return NULL;							\
    }else if(!c->next){							\
      next = list->header;						\
    }else if(c->next->next != list->header){				\
      next = c->next->next;						\
    }else{								\
      return NULL;							\
    }									\
    c->next = next;							\
    return (void*)&next->value;						\
  }

#define ulist_new_tpl(t)				\
  ulist_##t* ulist_##t##_new(){				\
    ulist_##t* list = ualloc(sizeof(ulist_##t));	\
    if(list){						\
      list->iterate = ulist_##t##_cursor_init;		\
      list->next = ulist_##t##_cursor_next;		\
      list->len = 0;					\
      list->header = NULL;				\
    }							\
    return list;					\
  }

#define ulist_append_tpl(t)				\
  int ulist_##t##_append(ulist_##t* list,t value){	\
    ulsnd_##t* header=list->header;			\
    ulsnd_##t* footer=NULL;				\
    ulsnd_##t* node=NULL;				\
    unew(node,sizeof(ulist_##t),return -1;)		\
      if(header==NULL){					\
	list->header=header=footer=node;		\
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
  ulist_cursor_tpl(t);				\
  ulist_cursor_next_tpl(t);			\
  ulist_new_tpl(t);				\
  ulist_append_tpl(t)