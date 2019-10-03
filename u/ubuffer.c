#include "ubuffer.h"

ubuffer* ubuffer_new(int size,URI_DECL){
  int _size = TYPE_SIZE_OF(ubuffer,char,size);
  ubuffer* buff;
  unew(buff,
       _size,
       URI_NILRET(UERR_C_OFM,UERR_M_OFM));
  buff->pos   = 0;
  buff->limit = 0;
  buff->size  = size;
  URI_ADDRET_SUCC(buff);
}

int ubuffer_read_from_buff(ubuffer* to_buff,
			   ubuffer* from_buff){
  int count = 0;
  while(to_buff->pos < to_buff->limit &&
	from_buff->pos < from_buff->limit){
    to_buff->data[to_buff->pos++] = from_buff->data[from_buff->pos++];
    count++;
  }
  return count;
}

int ubuffer_seed(ubuffer* buff,int offset,int origin,URI_DECL){
  int pos;
  if(origin == USEED_SET){
    pos = 0;
  }else if(origin == USEED_CUR){
    pos = buff->pos;
  }else if(origin == USEED_END){
    pos = buff->limit;
  }else{
    URI_VALRET(-100,"origin undefine!");
  }
  pos += offset;
  if(pos < 0){
    pos = 0;
  }else if(pos > buff->limit){
    pos = buff->limit;
  }
  buff->pos = pos;
  URI_VALRET_SUCC;
}
