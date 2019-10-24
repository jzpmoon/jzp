#include "ualloc.h"
#include "ustream.h"

#define USTREAM_FILE_BUFF_SIZE 10
#define USTREAM_FILE_BUFF_GET(stream) ((stream)->u.s.dbuff)
#define USTREAM_FILE_GET(stream) ((stream)->u.s.file)

ustream* ustream_new_by_buff(int iot,ubuffer* buff,URI_DECL){
  ustream* stream;
  unew(stream,sizeof(ustream),URI_RETVAL(UERR_OFM,NULL););
  stream->iot    = iot;
  stream->dst    = USTREAM_BUFF;
  stream->u.buff = buff;
  URI_RETVAL(UERR_SUCC,stream);
}

ustream* ustream_new_by_file(int iot,FILE* file,URI_DECL){
  ustream* stream;
  udbuffer* dbuff;
  unew(stream,sizeof(ustream),URI_RETVAL(UERR_OFM,NULL););
  dbuff = udbuffer_new(USTREAM_FILE_BUFF_SIZE);
  if(!dbuff){
    ufree(stream);
    URI_RETVAL(UERR_OFM,NULL);
  }
  stream->iot    = iot;
  stream->dst    = USTREAM_FILE;
  USTREAM_FILE_BUFF_GET(stream) = dbuff;
  USTREAM_FILE_GET(stream) = file;
  URI_RETVAL(UERR_SUCC,stream);
}

int ustream_read_to_buff(ustream* stream,ubuffer* buff,URI_DECL){
  int count = 0;
  if(stream->iot != USTREAM_INPUT){
    URI_RETVAL(UERR_IOT,count);
  }
  if(stream->dst == USTREAM_BUFF){
    count = ubuffer_read_from_buff(buff,stream->u.buff);
    URI_RETVAL(UERR_SUCC,count);
  }else if(stream->dst == USTREAM_FILE){
    FILE* file = USTREAM_FILE_GET(stream);
    count = ubuffer_read_from_file(buff,file);
    if(count == 0){
      URI_RETVAL(UERR_EOF,count);
    }else{
      URI_RETVAL(UERR_SUCC,count);
    }
  }else{
    URI_RETVAL(UERR_DST,count);
  }
}

int ustream_read_next(ustream* stream,URI_DECL){
  int next = -1;
  if(stream->iot != USTREAM_INPUT){
    URI_RETVAL(UERR_IOT,next);
  }
  if(stream->dst == USTREAM_BUFF){
    next = ubuffer_read_next(stream->u.buff);
    if(next == -1){
      URI_RETVAL(UERR_EOF,next);
    }
  }else if(stream->dst == USTREAM_FILE){
    udbuffer* dbuff = USTREAM_FILE_BUFF_GET(stream);
    FILE*     file  = USTREAM_FILE_GET(stream);
    next = udbuffer_read_next(dbuff);
    if(next == -1){
      udbuffer_read_from_file(dbuff,file);
      next = udbuffer_read_next(dbuff);
      if(next == -1){
	URI_RETVAL(UERR_EOF,next);	
      }
    }
  }else{
    URI_RETVAL(UERR_DST,next);
  }
  URI_RETVAL(UERR_SUCC,next);
}

int ustream_look_ahead(ustream* stream,URI_DECL){
  int next = -1;
  if(stream->iot != USTREAM_INPUT){
    URI_RETVAL(UERR_IOT,next);
  }
  if(stream->dst == USTREAM_BUFF){
    next = ubuffer_look_ahead(stream->u.buff);
    if(next == -1){
      URI_RETVAL(UERR_EOF,next);
    }
  }else if(stream->dst == USTREAM_FILE){
    udbuffer* dbuff = USTREAM_FILE_BUFF_GET(stream);
    FILE*     file  = USTREAM_FILE_GET(stream);
    next = udbuffer_look_ahead(dbuff);
    if(next == -1){
      udbuffer_read_from_file(dbuff,file);
      next = udbuffer_look_ahead(dbuff);
      if(next == -1){
	URI_RETVAL(UERR_EOF,next);	
      }
    }
  }else{
    URI_RETVAL(UERR_DST,next);
  }
  URI_RETVAL(UERR_SUCC,next);
}
