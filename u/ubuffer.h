#ifndef _UBUFFER_H_
#define _UBUFFER_H_

#include "uerror.h"

typedef struct _ubuffer{
  int pos;
  int limit;
  int size;
  char data[1];
} ubuffer;

ubuffer* ubuffer_new(int size,URI_DECL);

int ubuffer_read_from_buff(ubuffer* to_buff,
			   ubuffer* from_buff);

#define USEED_SET 0

#define USEED_CUR 1

#define USEED_END 2

int ubuffer_seed(ubuffer* buff,int offset,int origin,URI_DECL);

#define ubuffer_readly_read(buff)		\
  do{						\
    (buff)->limit = (buff)->pos;		\
    (buff)->pos   = 0;				\
  }while(0)

#define ubuffer_readly_write(buff)		\
  do{						\
    buff->limit = buff->size;			\
    buff->pos   = 0;				\
  }while(0)

#endif
