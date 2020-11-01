#include "ustream.h"

int main(int argc,char** args){
  URI_DEFINE;
  FILE* f;
  ustream* stream;
  int next;
  int b;
  if(argc == 1){
    ulog("no input file");
    return 0;
  }
  f = fopen(args[1],"r");
  if(!f){
    ulog1("open file %s error!",args[1]);
    return 0;
  }
  stream = ustream_new_by_file
    (USTREAM_INPUT,f,URI_REF);
  URI_ERROR;
  uabort(URI_DESC);
  URI_END;
  
  while(1){
    next = ustream_read_next(stream,URI_REF);
    if(next == -1){
      break;
    }
    URI_ERROR;
    uabort(URI_DESC);
    URI_END;
    b = next ^ 0xa3;
    putchar(b);
  } 
  
  return 0;
}
