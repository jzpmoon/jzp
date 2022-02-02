#include "ualloc.h"
#include "ustream.h"

#define USTREAM_FILE_BUFF_SIZE 10
#define USTREAM_FILE_BUFF_GET(stream) ((stream)->u.s.dbuff)
#define USTREAM_FILE_GET(stream) ((stream)->u.s.file)

ustring* ufile_path_name_get(uallocator* allocator,ustring* file_path)
{
  ustring* file_name;
  int pos;
  int len;

  pos = ustring_char_at(file_path,UDIR_SEP,-1);
  len = file_path->len - pos - 1;
  if (pos < 0) {
    pos = 0;
  }
  file_name = usubstring(allocator,file_path,pos,len);
  return file_name;
}

ustring* ufile_path_dir_get(uallocator* allocator,ustring* file_path)
{
  ustring* file_name;
  int pos;
  int len;

  pos = 0;
  len = ustring_char_at(file_path,UDIR_SEP,-1) + 1;
  file_name = usubstring(allocator,file_path,pos,len);
  return file_name;
}

ufile_infor* ufile_init(uallocator* allocator,ufile_infor* fi,
			ustring* file_path)
{
  ustring* file_name = NULL;
  ustring* dir_name = NULL;
  
  dir_name = ufile_path_dir_get(allocator,file_path);
  file_name = ufile_path_name_get(allocator,file_path);
  if (!file_name) {
    goto err;
  }
  fi->file_path = file_path;
  fi->dir_name = dir_name;
  fi->file_name = file_name;
  return fi;
 err:
  allocator->free(allocator,dir_name);
  allocator->free(allocator,file_name);
  return NULL;
}

void ufile_log(ufile_infor* fi)
{
  if (fi->file_path)
    ulog1("file path:%s",fi->file_path->value);
  if (fi->dir_name)
    ulog1("dir name:%s",fi->dir_name->value);
  if (fi->file_name)
    ulog1("file name:%s",fi->file_name->value);
}

ustream* ustream_new_by_buff(int iot,ubuffer* buff,URI_DECL){
  uallocator* allocator;
  ustream* stream;

  allocator = &u_global_allocator;
  stream = ustream_alloc(allocator,iot,USTREAM_BUFF);
  if (!stream) {
    URI_RETVAL(UERR_OFM,NULL);
  }
  URI_RETVAL(UERR_SUCC,stream);
}

ustream* ustream_new_by_file(int iot,ustring* file_path,URI_DECL)
{
  ustream* stream = NULL;
  FILE* file = NULL;
  char* mode;
  uallocator* allocator;

  allocator = &u_global_allocator;
  stream = ustream_alloc(allocator,iot,USTREAM_FILE);
  if (!stream) {
    goto err;
  }
  if(iot == USTREAM_INPUT){
    mode = "r";
  } else {
    mode = "w";
  }
  file = fopen(file_path->value,mode);
  if (!file) {
    goto err;
  }
  USTREAM_FILE_GET(stream) = file;
  URI_RETVAL(UERR_SUCC,stream);
 err:
  ustream_dest(stream);
  URI_RETVAL(UERR_OFM,NULL);
}

ustream* ustream_new_by_fd(int iot,FILE* fd,URI_DECL)
{
  ustream* stream = NULL;
  char* mode;
  uallocator* allocator;

  allocator = &u_global_allocator;
  stream = ustream_alloc(allocator,iot,USTREAM_FILE);
  if (!stream) {
    goto err;
  }
  if(iot == USTREAM_INPUT){
    mode = "r";
  } else {
    mode = "w";
  }
  USTREAM_FILE_GET(stream) = fd;
  URI_RETVAL(UERR_SUCC,stream);
 err:
  ustream_dest(stream);
  URI_RETVAL(UERR_OFM,NULL);
}

ustream* ustream_new(int iot,int dst){
  uallocator* allocator;

  allocator = &u_global_allocator;
  return ustream_alloc(allocator,iot,dst);
}

ustream* ustream_alloc(uallocator* allocator,int iot,int dst)
{
  ustream* stream = NULL;
  udbuffer* dbuff = NULL;

  stream = allocator->alloc(allocator,sizeof(ustream));
  if (stream) {
    stream->iot = iot;
    stream->dst = dst;
    stream->allocator = allocator;
    if (dst == USTREAM_BUFF) {
      stream->u.buff = NULL;
    } else if (dst == USTREAM_FILE) {
      if(stream->iot == USTREAM_INPUT){
	dbuff = udbuffer_alloc(allocator,USTREAM_FILE_BUFF_SIZE);
	if(!dbuff){
	  goto err;
	}
	stream->u.s.dbuff = dbuff;
      } else {
	stream->u.s.dbuff = NULL;
      }
      stream->u.s.file = NULL;
    }
  }
  return stream;
 err:
  allocator->free(allocator,stream);
  allocator->free(allocator,dbuff);
  return NULL;
}

void ustream_dest(ustream* stream)
{
  uallocator* alloc = stream->allocator;

  ustream_close(stream);
  alloc->free(alloc,stream->u.s.dbuff);
  alloc->free(alloc,stream);
}

int ustream_open_by_path(ustream* stream,
			 ustring* file_path)
{
  if (stream->dst == USTREAM_BUFF ||
      stream->iot == USTREAM_OUTPUT) {
    return -1;
  }
  udbuffer_empty(stream->u.s.dbuff);
  if (stream->u.s.file) {
    ustream_close(stream);
  }
  stream->u.s.file = fopen(file_path->value,"r");
  if (!stream->u.s.file) {
    return -1;
  } else {
    return 0;
  }
}

void ustream_close(ustream* stream)
{
  if (stream->dst == USTREAM_FILE) {
    FILE* file = stream->u.s.file;
    if (file) {
      fclose(file);
      stream->u.s.file = NULL;
    }
  }
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

int ustream_write_dnum(ustream* stream,double dnum,URI_DECL){
  if(stream->iot != USTREAM_OUTPUT){
    URI_RETVAL(UERR_IOT,-1);
  }
  if(stream->dst == USTREAM_BUFF){

  }else if(stream->dst == USTREAM_FILE){
    FILE* file = USTREAM_FILE_GET(stream);
    if(fprintf(file,"%f",dnum) < 0){
      URI_RETVAL(UERR_ERR,-1);
    }
  }else{
    URI_RETVAL(UERR_DST,-1);
  }
  URI_RETVAL(UERR_SUCC,0);
}

int ustream_write_int(ustream* stream, int inte,URI_DECL)
{
  if(stream->iot != USTREAM_OUTPUT){
    URI_RETVAL(UERR_IOT,-1);
  }
  if(stream->dst == USTREAM_BUFF){

  }else if(stream->dst == USTREAM_FILE){
    FILE* file = USTREAM_FILE_GET(stream);
    if(fprintf(file,"%d",inte) < 0){
      URI_RETVAL(UERR_ERR,-1);
    }
  }else{
    URI_RETVAL(UERR_DST,-1);
  }
  URI_RETVAL(UERR_SUCC,0);
}

int ustream_write_char(ustream* stream, int chara,URI_DECL)
{
  if(stream->iot != USTREAM_OUTPUT){
    URI_RETVAL(UERR_IOT,-1);
  }
  if(stream->dst == USTREAM_BUFF){

  }else if(stream->dst == USTREAM_FILE){
    FILE* file = USTREAM_FILE_GET(stream);
    if(fprintf(file,"%c",chara) < 0){
      URI_RETVAL(UERR_ERR,-1);
    }
  }else{
    URI_RETVAL(UERR_DST,-1);
  }
  URI_RETVAL(UERR_SUCC,0);
}

int ustream_write_string(ustream* stream,char* charp,URI_DECL)
{
  if(stream->iot != USTREAM_OUTPUT){
    URI_RETVAL(UERR_IOT,-1);
  }
  if(stream->dst == USTREAM_BUFF){

  }else if(stream->dst == USTREAM_FILE){
    FILE* file = USTREAM_FILE_GET(stream);
    if(fprintf(file,"%s",charp) < 0){
      URI_RETVAL(UERR_ERR,-1);
    }
  }else{
    URI_RETVAL(UERR_DST,-1);
  }
  URI_RETVAL(UERR_SUCC,0);
}
