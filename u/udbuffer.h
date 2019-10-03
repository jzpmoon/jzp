#ifndef _UDBUFFER_H_
#define _UDBUFFER_H_

#include "ubuffer.h"

typedef struct _udbuffer{
  ubuffer* buff_prev;
  ubuffer* buff_next;
  ubuffer* buff_curr;
  int      buff_size;
} udbuffer;

int udbuffer_read_next(udbuffer* dbuff);

int udbuffer_look_ahead(udbuffer* dbuff);

int udbuffer_read_from_file(udbuffer* dbuff,FILE* file);

#endif
