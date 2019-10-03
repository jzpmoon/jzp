#include "ustream.h"

int ustream_read_to_buff(ustrean* stream,ubuffer* buff,URI_DECL){
  int count = 0;
  if(stream->iot != USTREAM_INPUT){
    URI_SETRET(-100,"not a input stream!",count);
  }
  if(stream->dst == USTREAM_BUFF){
    count = ubuffer_read_from_buff(buff,stream->u.buff);
    URI_ADDRET_SUCC(count);
  }else if(stream->dst == USTREAM_FILE){
    FILE* file = stream->u.file;
    count = fread(buff->data + buff->pos,
		  1,
		  buff->size - buff->pos,
		  file);
    int eof_val = feof(file);
    int err_val = ferror(file);
    if(!eof && err_val){
      URI_SETRET(-101,"read file error!",count);
    }
    buff->pos += count;
    if(eof_val){
      URI_SETRET(-102,"end of file!",count);
    }else{
      URI_ADDRET_SUCC(count);
    }
  }else{
    URI_SETRET(-103,"unknow data source!",count);
  }
}
