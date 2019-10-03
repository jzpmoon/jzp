#include "ustream.h"

ustream* ustream_new_by_buff(int iot,ubuffer* buff,URI_DECL){
  ustream* stream;
  unew(stream,sizeof(ustream),URI_NILRET(URI_C_OFM,URI_M_OFM););
  stream->iot    = iot;
  stream->dst    = USTREAM_BUFF;
  stream->u.file = buff;
  URI_ADDRET_SUCC(stream);
}

ustream* ustream_new_by_file(int iot,FILE* file,URI_DECL){
  ustream* stream;
  unew(stream,sizeof(ustream),URI_NILRET(URI_C_OFM,URI_M_OFM););
  stream->iot    = iot;
  stream->dst    = USTREAM_FILE;
  stream->u.file = file;
  URI_ADDRET_SUCC(stream);
}

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

int ustream_read_next(ustream* stream,URI_DECL){
  int next = -1;
  if(stream->iot != USTREAM_INPUT){
    URI_SETRET(-100,"not a input stream!",next);
  }
  if(stream->dst == USTREAM_BUFF){
    next = ubuffer_read_next(stream->u.buff);
    if(next == -1){
      URI_SETRET(-102,"end of file!",next);
    }
  }else if(stream->dst == USTREAM_FILE){
    char c;
    int eof_val;
    int err_val;
    fread(&c,1,1,stream->u.file);
    eof_val = feof(file);
    err_val = ferror(file);
    if(eof_val){
      URI_SETRET(-102,"end of file!",next);
    }else{
      URI_SETRET(-101,"read file error!",next);
    }
    next = c;
  }else{
    URI_SETRET(-103,"unknow data source",next);
  }
  URI_ADDRET_SUCC(next);
}

int ustream_look_ahead(ustream* stream,URI_DECL){
  int next = -1;
  if(stream->iot != USTREAM_INPUT){
    URI_SETRET(-100,"not a input stream!",next);
  }
  if(stream->dst == USTREAM_BUFF){
    next = ubuffer_look_ahead(stream->u.buff);
    if(next == -1){
      URI_SETRET(-102,"end of file!",next);
    }
  }else if(stream->dst == USTREAM_FILE){
    char c;
    int eof_val;
    int err_val;
    fread(&c,1,1,stream->u.file);
    eof_val = feof(file);
    err_val = ferror(file);
    if(eof_val){
      URI_SETRET(-102,"end of file!",next);
    }else{
      URI_SETRET(-101,"read file error!",next);
    }
    if(fseek(stream->u.file,-1,SEEK_CUR) == -1){
      URI_SETRET(-101,"read file error!",next);
    }
    next = c;
  }else{
    URI_SETRET(-103,"unknow data source",next);
  }
  URI_ADDRET_SUCC(next);
}
