#ifndef _USTACK_TPL_H_
#define _USTACK_TPL_H_

#define ustack_tpl(t)				\
  typedef struct _ustack##t{			\
    struct _ublock##t* curr_block;		\
    struct _ublock##t* cache_block;		\
    int block_pos;				\
    int block_count;				\
    int block_limit;				\
  } ustack##t;

#ifndef ustack_push_tpl
#define ustack_push_tpl(t) \
  int ustack_push##t(ustack##t* stack,t data);
#endif

#ifndef ustack_pop_tpl
#define ustack_pop_tpl(t) \
  int ustack_pop##t(ustack##t* stack,t* data);
#endif

#endif
