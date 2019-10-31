#include <stdio.h>
#include "ltoken.h"
#include "lobj.h"

ltoken_state ts;

int main(){
  URI_DEFINE;
  vgc_heap* heap;
  vgc_stack* stack;
  vslot* slot;
  FILE* file;
  ustream* stream;
  ubuffer* buff;
  
  heap=vgc_heap_new(256,1024*10);
  if(!heap){
    uabort("heap new error");
  }
  stack=vgc_stack_new(heap,100);
  if(!stack){
    uabort("stack new error");
  }
  
  file = fopen("./test.txt","r");
  if(!file){
    uabort("open file error");
  }

  stream = ustream_new_by_file(USTREAM_INPUT,file,URI_REF);
  URI_ERROR;
  uabort(URI_DESC);
  URI_END;

  ts.stream = stream;

  buff = ubuffer_new(100);
  if(!buff){
    uabort("new buffer error");
  }
  ubuffer_ready_write(buff);
  ts.buff = buff;
  
  slot = lparser_parse(&ts,heap,stack);

  lparser_exp_log(*slot);
  /*while(1){
    token = ltoken_next(&ts,heap);
    ltoken_log(&ts);
    if(token == ltk_eof || token == ltk_bad)
      break;
  }*/
  return 0;
}
