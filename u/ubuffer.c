#include "umacro.h"
#include "ualloc.h"
#include "ubuffer.h"

uapi ubuffer* ucall ubuffer_new(int size)
{
  int _size;
  ubuffer* buff;
  uallocator* allocator;

  _size = TYPE_SIZE_OF(ubuffer,char,size);
  allocator = &u_global_allocator;
  buff = allocator->alloc(allocator,_size);
  if (buff) {
    buff->allocator = allocator;
    buff->pos   = 0;
    buff->limit = 0;
    buff->size  = size;
  }
  return buff;
}

uapi ubuffer* ucall ubuffer_alloc(uallocator* allocator,int size)
{
  int _size;
  ubuffer* buff;

  _size = TYPE_SIZE_OF(ubuffer,char,size);
  buff = allocator->alloc(allocator,_size);
  if (buff) {
    buff->allocator = allocator;
    buff->pos   = 0;
    buff->limit = 0;
    buff->size  = size;
  }
  return buff;
}

uapi void ucall ubuffer_dest(ubuffer* buff)
{
  buff->allocator->free(buff->allocator,buff);
}

uapi int ucall ubuffer_read_from_buff(ubuffer* to_buff,
			   ubuffer* from_buff){
  int count = 0;
  while(to_buff->pos < to_buff->limit &&
	from_buff->pos < from_buff->limit){
    to_buff->data[to_buff->pos++] = from_buff->data[from_buff->pos++];
    count++;
  }
  return count;
}

uapi int ucall ubuffer_read_from_file(ubuffer* to_buff,
			   FILE*    from_file){
  int count = 0;
  int eof_val;
  int err_val;
  int len   = to_buff->limit - to_buff->pos;
  if(len > 0){
    count = fread(&to_buff->data[to_buff->pos],1,len,from_file);
    eof_val = feof(from_file);
    err_val = ferror(from_file);
    if(!eof_val && err_val){
      uabort("read file error!");
    }
    to_buff->pos += count;
    to_buff->limit   = to_buff->pos;
  }
  return count;
}

uapi int ucall ubuffer_read_next(ubuffer* buff){
  int next;
  if(buff->pos < buff->limit){
    next = buff->data[buff->pos++];
  }else{
    next = -1;
  }
  return next;
}

uapi int ucall ubuffer_write_next(ubuffer* buff,int byte){
  if(buff->pos < buff->limit){
    buff->data[buff->pos++] = byte;
    return 0;
  }else{
    return -1;
  }
}

uapi int ucall ubuffer_look_ahead(ubuffer* buff){
  int next;
  if(buff->pos < buff->limit){
    next = buff->data[buff->pos];
  }else{
    next = -1;
  }
  return next;
}

uapi int ucall ubuffer_seek(ubuffer* buff,int offset,int origin){
  int pos;
  if(origin == USEEK_SET){
    pos = 0;
  }else if(origin == USEEK_CUR){
    pos = buff->pos;
  }else if(origin == USEEK_END){
    pos = buff->limit;
  }else{
    return -1;
  }
  pos += offset;
  if(pos < 0){
    pos = 0;
  }else if(pos > buff->limit){
    pos = buff->limit;
  }
  buff->pos = pos;
  return 0;
}

