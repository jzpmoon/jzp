#include "ualloc.h"
#include "udbuffer.h"

udbuffer* udbuffer_new(int size){
  udbuffer* dbuff;
  unew(dbuff,sizeof(udbuffer),return NULL;);
  dbuff->buff_prev = NULL;
  dbuff->buff_next = NULL;
  dbuff->buff_curr = NULL;
  dbuff->buff_size = size;
  return dbuff;
}

int udbuffer_read_next(udbuffer* dbuff){
  int c;
  ubuffer* buff;
  if(!dbuff->buff_curr){
    return -1;
  }
  buff = dbuff->buff_curr;
  
  c = ubuffer_read_next(buff);
  if(c == -1){
    if(buff == dbuff->buff_prev){
      if(!dbuff->buff_next||
	 ubuffer_is_empty(dbuff->buff_next)){
	return -1;
      }else{
	buff = dbuff->buff_next;
      }
    }else{
      dbuff->buff_next = dbuff->buff_prev;
      dbuff->buff_prev = buff;
      ubuffer_empty(dbuff->buff_next);
      return -1;
    }
  }else{
    return c;
  }
  
  c = ubuffer_read_next(buff);
  dbuff->buff_curr = buff;
  return c;
}

int udbuffer_look_ahead(udbuffer* dbuff){
  int c;
  ubuffer* buff;
  if(!dbuff->buff_curr){
    return -1;
  }
  buff = dbuff->buff_curr;
  
  c = ubuffer_look_ahead(buff);
  if(c == -1){
    if(buff == dbuff->buff_prev){
      if(!dbuff->buff_next||
	 ubuffer_is_empty(dbuff->buff_next)){
	return -1;
      }else{
	buff = dbuff->buff_next;
      }
    }else{
      dbuff->buff_next = dbuff->buff_prev;
      dbuff->buff_prev = buff;
      ubuffer_empty(dbuff->buff_next);
      return -1;
    }
  }else{
    return c;
  }
  
  c = ubuffer_look_ahead(buff);
  dbuff->buff_curr = buff;
  return c;
}

ubuffer* udbuffer_empty_buff_get(udbuffer* dbuff,
				 ubuffer* buff){
  ubuffer* buff_empty = NULL;
  if(!buff){
    buff_empty = ubuffer_new(dbuff->buff_size);
    if(!buff_empty){
      return NULL;
    }
    ubuffer_ready_write(buff_empty);
  }else if(ubuffer_is_empty(buff)){
    buff_empty = buff;
    ubuffer_ready_write(buff_empty);
  }
  return buff_empty;
}

int udbuffer_read_from_file(udbuffer* dbuff,FILE* file){
  int count = 0;
  ubuffer* buff = udbuffer_empty_buff_get(dbuff,dbuff->buff_prev);
  if(buff){
    dbuff->buff_prev = buff;
    if(!dbuff->buff_curr){
      dbuff->buff_curr = buff;
    }
  }else{
    buff = udbuffer_empty_buff_get(dbuff,dbuff->buff_next);
    if(buff){
      dbuff->buff_next = buff;  
    }
  }

  if(buff){
    count = ubuffer_read_from_file(buff,file);
    ubuffer_ready_read(buff);
  }
  return count;
}

void udbuffer_empty(udbuffer* dbuff){
  if (dbuff->buff_prev) {
    ubuffer_empty(dbuff->buff_prev);
  }
  if (dbuff->buff_next) {
    ubuffer_empty(dbuff->buff_next);
  }
  dbuff->buff_curr = dbuff->buff_prev;
}
