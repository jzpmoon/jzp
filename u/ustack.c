#include <stddef.h>
#include "ustack.h"
#include "ualloc.h"

#define BLOCK_SIZE 4*1024

typedef struct _ublock{
  struct _ublock* next;
  void* ptr[BLOCK_SIZE];
} ublock;

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

static ublock* ublock_push(ustack* stack){
  ublock* block;
  if(_IS_BLOCK_FULL(stack)){
    if(_IS_STACK_FULL(stack)){
      return NULL;
    }
    if(!(block=stack->cache_block)){
      unew(block,
	   sizeof(ublock),
	   return NULL;);
      stack->block_count++;
    }
    block->next=stack->curr_block;
    stack->curr_block=block;
    stack->block_pos=0;
  }
  return stack->curr_block;
}

int ustack_push(ustack* stack,
		void* data){
  ublock* block;
  if((block=ublock_push(stack))!=NULL){
    block->ptr[stack->block_pos++]=data;
    return 0;
  }
  return -1;
}

static ublock* ublock_pop(ustack* stack){
  if(_IS_BLOCK_EMPTY(stack)){
    if(!_IS_STACK_EMPTY(stack)){
      ublock* block=stack->curr_block;
      stack->curr_block=
	block->next;
      if(stack->cache_block){
	ufree(block);
	stack->block_count--;
      }else{
	stack->cache_block=block;
      }
      if(stack->curr_block)
	stack->block_pos=BLOCK_SIZE;
    }
  }
  return stack->curr_block;
}

int ustack_pop(ustack* stack,
	       void** data){
  ublock* block;
  if((block=ublock_pop(stack))!=NULL){
    *data=
      block->ptr[--(stack->block_pos)];
    return 0;
  }
  return -1;
}
