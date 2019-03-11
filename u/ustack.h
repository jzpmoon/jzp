#ifndef _USTACK_H_
#define _USTACK_H_

typedef struct _ustack{
  struct _ublock* curr_block;
  struct _ublock* cache_block;
  int block_pos;
  int block_count;
  int block_limit;
} ustack;

#define ustack_push_obj(s,o) \
  ustack_push(s,(void*)(o))

#define ustack_pop_obj(s,o) \
  ustack_pop(s,(void**)(&(o)))

int ustack_push(ustack* stack,
		void* data);
int ustack_pop(ustack* stack,
	       void** data);

#endif
