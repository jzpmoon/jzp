#ifndef _UBUFFER_H_
#define _UBUFFER_H_

#include "uerror.h"
#include "ualloc.h"

typedef struct _ubuffer{
  uallocator* allocator;
  int pos;
  int limit;
  int size;
  char data[1];
} ubuffer;

uapi ubuffer* ucall ubuffer_new(int size);

uapi ubuffer* ucall ubuffer_alloc(uallocator* allocator,int size);

uapi void ucall ubuffer_dest(ubuffer* buff);

uapi int ucall ubuffer_read_from_buff(ubuffer* to_buff,
			   ubuffer* from_buff);

uapi int ucall ubuffer_read_from_file(ubuffer* to_buff,
			   FILE*    from_file);

uapi int ucall ubuffer_read_next(ubuffer* buff);

uapi int ucall ubuffer_look_ahead(ubuffer* buff);

uapi int ucall ubuffer_write_next(ubuffer* buff,int byte);

#define USEEK_SET 0

#define USEEK_CUR 1

#define USEEK_END 2

uapi int ucall ubuffer_seek(ubuffer* buff,int offset,int origin);

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
