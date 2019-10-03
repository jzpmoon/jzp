#include "udbuffer.h"

int udbuffer_read_next(udbuffer* dbuff){
  int next;
  ubuffer* buff;
  if(!dbuff->buff_curr){
    return -1;
  }
  buff = dbuff->buff_curr;
  
  next = ubuffer_read_next(buff);
  if(next == -1){
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
    return next;
  }
  
  next = ubuffer_read_next(buff);
  dbuff->buff_curr = buff;
  return next;
}

int udbuffer_look_ahead(udbuffer* dbuff){
  int next;
  ubuffer* buff;
  if(!dbuff->buff_curr){
    return -1;
  }
  buff = dbuff->buff_curr;
  
  next = ubuffer_look_ahead(buff);
  if(next == -1){
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
    return next;
  }
  
  next = ubuffer_look_ahead(buff);
  return next;
}

ubuffer* udbuffer_empty_buff_get(udbuffer* dbuff,
				 ubuffer* buff){
  URI_DEFINE;
  ubuffer* buff_empty = NULL;
  if(!buff){
    buff_empty = ubuffer_new(dbuff->buff_size,URI_REF);
    URI_ERROR;
    return 0;
    URI_END;
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
