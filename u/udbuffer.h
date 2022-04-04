#ifndef _UDBUFFER_H_
#define _UDBUFFER_H_

#include "ubuffer.h"

typedef struct _udbuffer{
  ubuffer* buff_prev;
  ubuffer* buff_next;
  ubuffer* buff_curr;
  int      buff_size;
} udbuffer;

uapi udbuffer* ucall udbuffer_new(int size);

uapi udbuffer* ucall udbuffer_alloc(uallocator* allocator,int size);

uapi int ucall udbuffer_read_next(udbuffer* dbuff);

uapi int ucall udbuffer_look_ahead(udbuffer* dbuff);

uapi int ucall udbuffer_read_from_file(udbuffer* dbuff,FILE* file);

uapi void ucall udbuffer_empty(udbuffer* dbuff);

#endif
