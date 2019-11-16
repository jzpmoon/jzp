#ifndef _USTACK_TPL_H_
#define _USTACK_TPL_H_

#define ustack_tpl(t)				\
  typedef struct _ustack_##t{			\
    struct _ublock_##t* curr_block;		\
    struct _ublock_##t* cache_block;		\
    int block_pos;				\
    int block_count;				\
    int block_limit;				\
    int block_size;				\
  } ustack_##t;

#ifndef ustack_push_tpl
#define ustack_push_tpl(t)				\
  int ustack_push_##t(ustack_##t* stack,t data);
#endif

#ifndef ustack_pop_tpl
#define ustack_pop_tpl(t)				\
  int ustack_pop_##t(ustack_##t* stack,t* data);
#endif

#define ustack_init(t,stack,limit,size)			\
  stack = (ustack_##t){NULL,NULL,0,0,limit,size}

#endif
