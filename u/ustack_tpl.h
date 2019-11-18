#ifndef _USTACK_TPL_H_
#define _USTACK_TPL_H_

#include "ualloc.h"

#define ustack_tpl(t)				\
  typedef struct _ustack_##t{			\
    struct _ublock_##t* curr_block;		\
    struct _ublock_##t* cache_block;		\
    int block_pos;				\
    int block_count;				\
    int block_limit;				\
    int block_size;				\
  } ustack_##t;

#define BLOCK_SIZE 4*1024


#define ustack_push_decl_tpl(t)				\
  int ustack_push_##t(ustack_##t* stack,t data);

#define ustack_pop_decl_tpl(t)				\
  int ustack_pop_##t(ustack_##t* stack,t* data);

#define UBLOCK_SIZE_GET(SIZE)			\
  SIZE > 0 ? SIZE : BLOCK_SIZE

#define ustack_init(t,stack,limit,size)			\
  do{							\
    (stack)->curr_block = NULL;				\
    (stack)->cache_block = NULL;			\
    (stack)->block_pos = 0;				\
    (stack)->block_count = 0;				\
    (stack)->block_limit = limit;			\
    (stack)->block_size = UBLOCK_SIZE_GET(size);	\
  } while(0)

#endif
