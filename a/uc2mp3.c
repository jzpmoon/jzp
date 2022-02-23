#include "ustream.h"

int main(int argc,char** args){
  URI_DEFINE;
  ustring* file_path;
  ustream* stream;
  int next;
  int b;
  if(argc == 1){
    ulog("no input file");
    return 0;
  }
  file_path = ustring_new_by_charp(NULL,args[1]);
  if (!file_path) {
    uabort("ustring new error!");
  }
  stream = ustream_new_by_file
    (USTREAM_INPUT,file_path,URI_REF);
  URI_ERROR;
  uabort(URI_DESC);
  URI_END;
  
  while(1){
    next = ustream_read_next(stream,URI_REF);
    URI_ERROR
      URI_CASE(UERR_EOF)
	break;
      URI_ELSE
	uabort(URI_DESC);
      URI_END
    URI_END
    b = next ^ 0xa3;
    putchar(b);
  } 
  
  return 0;
}
