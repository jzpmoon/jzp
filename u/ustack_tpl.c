#include <stddef.h>
#include "ualloc.h"
#include "uerror.h"
#include "ustack_tpl.h"

#define _IS_BLOCK_FULL(S)			\
  ((S)->block_pos>=(S)->block_size||		\
   !(S)->curr_block)
#define _IS_STACK_FULL(S)			\
  ((S)->block_count==				\
   (S)->block_limit)
#define _IS_BLOCK_EMPTY(S)			\
  ((S)->block_pos==0)
#define _IS_STACK_EMPTY(S)			\
  (!(S)->curr_block)

#define ustack_log_tpl(t)					\
  uapi_tpl void ucall ustack_##t##_log(ustack_##t* stack){	\
    ulog1("ustack curr_block: %p",stack->curr_block);		\
    ulog1("ustack cache_block:%p",stack->cache_block);		\
    ulog1("ustack block_pos:  %d",stack->block_pos);		\
    ulog1("ustack block_count:%d",stack->block_count);		\
    ulog1("ustack block_limit:%d",stack->block_limit);		\
    ulog1("ustack block_size:%d",stack->block_size);		\
  }

#define ublock_new_tpl(t)					\
  static ublock_##t* ublock_##t##_new(int block_size){		\
    int size = TYPE_SIZE_OF(ublock_##t,t,block_size);		\
    ublock_##t* block;						\
    unew(block,size,return NULL;);				\
    block->next = NULL;						\
    return block;						\
  }

#define ublock_push_tpl(t)					\
  static ublock_##t* ublock_##t##_push(ustack_##t* stack){	\
    ublock_##t* block;						\
    if(_IS_BLOCK_FULL(stack)){					\
      if(_IS_STACK_FULL(stack)){				\
	return NULL;						\
      }								\
      if(!(block=stack->cache_block)){				\
	block = ublock_##t##_new(stack->block_size);		\
	if(!block){						\
	  return NULL;						\
	}							\
	stack->block_count++;					\
      }								\
      block->next=stack->curr_block;				\
      stack->curr_block=block;					\
      stack->cache_block=NULL;					\
      stack->block_pos=0;					\
    }								\
    return stack->curr_block;					\
  }

#define ustack_push_tpl(t)					\
  uapi_tpl int ucall ustack_##t##_push(ustack_##t* stack,	\
				       t data){			\
    ublock_##t* block;						\
    if((block=ublock_##t##_push(stack))!=NULL){			\
      block->ptr[stack->block_pos++]=data;			\
      return 0;							\
    }								\
    return -1;							\
  }

#define ustack_pushv_tpl(t)					\
  uapi_tpl int ucall ustack_##t##_pushv(ustack_##t* stack){	\
    ublock_##t* block;						\
    if((block=ublock_##t##_push(stack))!=NULL){			\
      stack->block_pos++;					\
      return 0;							\
    }								\
    return -1;							\
  }

#define ustack_set_tpl(t)					\
  uapi_tpl int ucall ustack_##t##_set(ustack_##t* stack,	\
				      int index,t data){	\
    if(index < 0 || index >= stack->block_pos){			\
      return -1;						\
    }								\
    stack->curr_block->ptr[index] = data;			\
    return 0;							\
  }

#define ustack_get_tpl(t)					\
  uapi_tpl int ucall ustack_##t##_get(ustack_##t* stack,	\
				      int index,t* data){	\
    int top = index;						\
    if(top < 0){						\
      top += stack->block_pos;					\
    }								\
    if(top < 0 || top >= stack->block_pos){			\
      return -1;						\
    }								\
    *data = stack->curr_block->ptr[top];			\
    return 0;							\
  }

#define ustack_top_set_tpl(t)					\
  uapi_tpl int ucall ustack_##t##_top_set(ustack_##t* stack,	\
					  int index){		\
    int top = index;						\
    if(top < 0){						\
      top += stack->block_pos;					\
    }								\
    if(top < 0 || top >= stack->block_size){			\
      return -1;						\
    }								\
    stack->block_pos = top;					\
    return 0;							\
  }

#define ustack_top_get_tpl(t)					\
  uapi_tpl int ucall ustack_##t##_top_get(ustack_##t* stack){	\
    return stack->block_pos;					\
  }

#define ublock_pop_tpl(t)					\
  static ublock_##t* ublock_##t##_pop(ustack_##t* stack){	\
    if(_IS_BLOCK_EMPTY(stack)){					\
      if(!_IS_STACK_EMPTY(stack)){				\
	ublock_##t* block=stack->curr_block;			\
	stack->curr_block=					\
	  block->next;						\
	if(stack->cache_block){					\
	  ufree(block);						\
	  stack->block_count--;					\
	}else{							\
	  stack->cache_block=block;				\
	}							\
	if(stack->curr_block)					\
	  stack->block_pos=stack->block_size;			\
      }								\
    }								\
    return stack->curr_block;					\
  }

#define ustack_pop_tpl(t)					\
  uapi_tpl int ucall ustack_##t##_pop(ustack_##t* stack,	\
				      t* data){			\
    ublock_##t* block;						\
    if((block=ublock_##t##_pop(stack))!=NULL){			\
      *data=							\
	block->ptr[--(stack->block_pos)];			\
      return 0;							\
    }								\
    return -1;							\
  }

#define ustack_def_tpl(t)			\
  ustack_log_tpl(t)				\
  ublock_new_tpl(t)				\
  ublock_push_tpl(t)				\
  ustack_push_tpl(t)				\
  ustack_pushv_tpl(t)				\
  ustack_set_tpl(t)				\
  ustack_get_tpl(t)				\
  ustack_top_set_tpl(t)			        \
  ustack_top_get_tpl(t)			        \
  ublock_pop_tpl(t)				\
  ustack_pop_tpl(t)
