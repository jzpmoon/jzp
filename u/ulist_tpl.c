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

#define ulist_new_tpl(t)						\
  ulist_##t* ulist_##t##_new(){						\
    uallocator* allocator = &u_global_allocator;			\
    ulist_##t* list = allocator->alloc(allocator,sizeof(ulist_##t));	\
    if(list){								\
      list->iterate = ulist_##t##_cursor_init;				\
      list->next = ulist_##t##_cursor_next;				\
      list->allocator = allocator;					\
      list->len = 0;							\
      list->header = NULL;						\
    }									\
    return list;							\
  }

#define ulist_alloc_tpl(t)					\
  ulist_##t* ulist_##t##_alloc(uallocator* allocator){		\
    ulist_##t* list;						\
								\
    list = allocator->alloc(allocator,sizeof(ulist_##t));	\
    if(list){							\
      list->iterate = ulist_##t##_cursor_init;			\
      list->next = ulist_##t##_cursor_next;			\
      list->allocator = allocator;				\
      list->len = 0;						\
      list->header = NULL;					\
    }								\
    return list;						\
  }

#define ulist_append_tpl(t)						\
  int ulist_##t##_append(ulist_##t* list,t value){			\
    ulsnd_##t* header=list->header;					\
    ulsnd_##t* footer=NULL;						\
    ulsnd_##t* node=NULL;						\
									\
    node = list->allocator->alloc(list->allocator,sizeof(ulsnd_##t));	\
    if (!node) {							\
      return -1;							\
    }									\
    if(header==NULL){							\
      list->header=header=footer=node;					\
    }else{								\
      footer=header->prev;						\
    }									\
    header->prev=node;							\
    footer->next=node;							\
    node->prev=footer;							\
    node->next=header;							\
    node->value=value;							\
    list->len++;							\
    return 0;								\
  }

#define ulist_concat_tpl(t)					\
  void ulist_##t##_concat(ulist_##t* list1,ulist_##t* list2){	\
    ulsnd_##t* header1 = list1->header;				\
    ulsnd_##t* header2 = list2->header;				\
    ulsnd_##t* footer1;						\
    ulsnd_##t* footer2;						\
    if(!header1 || !header2){					\
      return;							\
    }								\
    footer1 = header1->prev;					\
    footer2 = header2->prev;					\
    header1->prev = footer2;					\
    footer2->next = header1;					\
    header2->prev = footer1;					\
    footer1->next = header2;					\
    list1->len += list2->len;					\
  }

#define ulist_dest_tpl(t)						\
  void ulist_##t##_dest(ulist_##t* list,ulist_##t##_dest_ft dest){	\
    uallocator* allocator = list->allocator;				\
    ulsnd_##t* header = list->header;					\
    ulsnd_##t* nd = list->header;					\
    if (nd) {								\
      do {								\
	ulsnd_##t* temp = nd;						\
	nd = nd->next;							\
	if (dest) {							\
	  dest(temp->value);						\
	}								\
	allocator->free(allocator,temp);				\
      }	while (nd->next != header);					\
    }									\
    allocator->free(allocator,list);					\
  }

#define ulist_def_tpl(t)			\
  ulist_cursor_tpl(t);				\
  ulist_cursor_next_tpl(t);			\
  ulist_new_tpl(t);				\
  ulist_alloc_tpl(t);				\
  ulist_append_tpl(t);				\
  ulist_concat_tpl(t);				\
  ulist_dest_tpl(t)
