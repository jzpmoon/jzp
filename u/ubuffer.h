#ifndef _UBUFFER_H_
#define _UBUFFER_H_

#include "uerror.h"

typedef struct _ubuffer{
  int pos;
  int limit;
  int size;
  char data[1];
} ubuffer;

ubuffer* ubuffer_new(int size);

int ubuffer_read_from_buff(ubuffer* to_buff,
			   ubuffer* from_buff);

int ubuffer_read_from_file(ubuffer* to_buff,
			   FILE*    from_file);

int ubuffer_read_next(ubuffer* buff);

int ubuffer_look_ahead(ubuffer* buff);

int ubuffer_write_next(ubuffer* buff,int byte);

#define USEEK_SET 0

#define USEEK_CUR 1

#define USEEK_END 2

int ubuffer_seek(ubuffer* buff,int offset,int origin);

#define ubuffer_ready_read(buff)		\
  do{						\
    (buff)->limit = (buff)->pos;		\
    (buff)->pos   = 0;				\
  }while(0)

#define ubuffer_ready_write(buff)		\
  do{						\
    (buff)->limit = (buff)->size;		\
    (buff)->pos   = 0;				\
  }while(0)

#define ubuffer_empty(buff) \
  ((buff)->pos = 0,(buff)->limit = 0)

#define ubuffer_stock(buff) \
  ((buff)->limit)

#define ubuffer_has_next(buff) \
  ((buff)->limit - (buff)->pos > 0)

#define ubuffer_is_empty(buff) \
  ((buff)->pos == 0 && (buff)->limit == 0)

#endif
