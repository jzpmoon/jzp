#include <stddef.h>
#include "ustack.h"
#include "ualloc.h"
#include "uerror.h"

#define BLOCK_SIZE 4*1024

#define ublock_tpl(t)				\
  typedef struct _ublock##t{			\
    struct _ublock##t* next;			\
    t ptr[BLOCK_SIZE];				\
  } ublock##t;

#define _IS_BLOCK_FULL(S)       \
  ((S)->block_pos>=BLOCK_SIZE||	\
   !(S)->curr_block)
#define _IS_STACK_FULL(S)       \
  ((S)->block_count==		\
   (S)->block_limit)
#define _IS_BLOCK_EMPTY(S)      \
  ((S)->block_pos==0)
#define _IS_STACK_EMPTY(S)      \
  (!(S)->curr_block)

#define ustack_log_tpl(t)					\
  void ustack_log##t(ustack##t* stack){				\
    ulog1("ustack curr_block: %ld",(long)stack->curr_block);	\
    ulog1("ustack cache_block:%ld",(long)stack->cache_block);	\
    ulog1("ustack block_pos:  %d",stack->block_pos);		\
    ulog1("ustack block_count:%d",stack->block_count);		\
    ulog1("ustack block_limit:%d",stack->block_limit);		\
  }

#define ublock_push_tpl(t)				\
  static ublock##t* ublock_push##t(ustack##t* stack){	\
    ublock##t* block;					\
    if(_IS_BLOCK_FULL(stack)){				\
      if(_IS_STACK_FULL(stack)){			\
	return NULL;					\
      }							\
      if(!(block=stack->cache_block)){			\
	unew(block,					\
	     sizeof(ublock##t),				\
	     return NULL;);				\
	stack->block_count++;				\
      }							\
      block->next=stack->curr_block;			\
      stack->curr_block=block;				\
      stack->cache_block=NULL;				\
      stack->block_pos=0;				\
    }							\
    return stack->curr_block;				\
  }

#define ustack_push_tpl(t)			\
  int ustack_push##t(ustack##t* stack,		\
		     t data){			\
    ublock##t* block;				\
    if((block=ublock_push##t(stack))!=NULL){	\
      block->ptr[stack->block_pos++]=data;	\
      return 0;					\
    }						\
    return -1;					\
  }

#define ublock_pop_tpl(t)				\
  static ublock##t* ublock_pop##t(ustack##t* stack){	\
    if(_IS_BLOCK_EMPTY(stack)){				\
      if(!_IS_STACK_EMPTY(stack)){			\
	ublock##t* block=stack->curr_block;		\
	stack->curr_block=				\
	  block->next;					\
	if(stack->cache_block){				\
	  ufree(block);					\
	  stack->block_count--;				\
	}else{						\
	  stack->cache_block=block;			\
	}						\
	if(stack->curr_block)				\
	  stack->block_pos=BLOCK_SIZE;			\
      }							\
    }							\
    return stack->curr_block;				\
  }

#define ustack_pop_tpl(t)			\
  int ustack_pop##t(ustack##t* stack,		\
		    t* data){			\
    ublock##t* block;				\
    if((block=ublock_pop##t(stack))!=NULL){	\
      *data=					\
	block->ptr[--(stack->block_pos)];	\
      return 0;					\
    }						\
    return -1;					\
  }
