#ifndef _USTACK_TPL_H_
#define _USTACK_TPL_H_

#include "umacro.h"
#include "ualloc.h"

#define ublock_tpl(t)				\
  typedef struct _ublock_##t{			\
    struct _ublock_##t* next;			\
    t ptr[1];					\
  } ublock_##t;

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

#define ustack_log_decl_tpl(t)			\
  uapi_tpl void ucall ustack_##t##_log(ustack_##t* stack);

#define ustack_push_decl_tpl(t)				\
  uapi_tpl int ucall ustack_##t##_push(ustack_##t* stack,t data);

#define ustack_pushv_decl_tpl(t)			\
  uapi_tpl int ucall ustack_##t##_pushv(ustack_##t* stack);

#define ustack_pop_decl_tpl(t)				\
  uapi_tpl int ucall ustack_##t##_pop(ustack_##t* stack,t* data);

#define ustack_set_decl_tpl(t)				\
  uapi_tpl int ucall ustack_##t##_set(ustack_##t* stack,int index,t data);

#define ustack_get_decl_tpl(t)				\
  uapi_tpl int ucall ustack_##t##_get(ustack_##t* stack,int index,t* data);

#define ustack_top_set_decl_tpl(t)			\
  uapi_tpl int ucall ustack_##t##_top_set(ustack_##t* stack,int index);

#define ustack_top_get_decl_tpl(t)			\
  uapi_tpl int ucall ustack_##t##_top_get(ustack_##t* stack);

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

#define ustack_decl_tpl(t)			\
  ublock_tpl(t)					\
  ustack_tpl(t)					\
  ustack_log_decl_tpl(t)			\
  ustack_push_decl_tpl(t)			\
  ustack_pushv_decl_tpl(t)			\
  ustack_set_decl_tpl(t)			\
  ustack_get_decl_tpl(t)			\
  ustack_top_set_decl_tpl(t)			\
  ustack_top_get_decl_tpl(t)			\
  ustack_pop_decl_tpl(t)

#endif
