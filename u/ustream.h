#ifndef _USTREAM_H_
#define _USTREAM_H_

#include "ubuffer.h"

#define USTREAM_INPUT  0
#define USTREAM_OUTPUT 1

#define USTREAM_BUFF   0
#define USTREAM_FILE   1

typedef struct _ustream{
  unsigned int iot:1;
  unsigned int dst:1;
  union {
    ubuffer* buff;
    FILE*    file;
  } u;
} stream;

ustream* ustream_new_by_buff(int iot,ubuffer* buff,URI_DECL);

ustream* ustream_new_by_file(int iot,FILE* file,URI_DECL);

int ustream_read_to_buff(ustrean* stream,ubuffer* buff,URI_DECL);

int ustream_read_next(ustream* stream,URI_DECL);

int ustream_look_ahead(ustream* stream,URI_DECL);

#endif
