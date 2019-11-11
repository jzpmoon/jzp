#include <stdio.h>
#include "vvm.h"
#include "ltoken.h"
#include "lobj.h"
#include "leval.h"

ltoken_state ts;

int test_1(){
  URI_DEFINE;
  vgc_heap* heap;
  vgc_stack* stack;
  vslot* slot;
  FILE* file;
  ustream* stream;
  ubuffer* buff;
  
  heap=vgc_heap_new(1024*10,1024*10);
  if(!heap){
    uabort("heap new error");
  }
  stack=vgc_stack_new(heap,NULL,100,area_static);
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

  lparser_exp_log(slot);
  /*while(1){
    token = ltoken_next(&ts,heap);
    ltoken_log(&ts);
    if(token == ltk_eof || token == ltk_bad)
      break;
  }*/
  return 0;
}

int test_2(){
  vvm* vm = vvm_new(1024*10,
		    1028*10,
		    100,
		    100);
  vgc_heap* heap = vm->heap;
  vcontext* ctx = vm->context;
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  vslot* slotp_str = vgc_str_newc(heap,stack,"./test.txt",10,area_active);
  if(!slotp_str){
    uabort("test_2:str new error!");
  }
  lstartup(vm,slotp_str);
  return 0;
}

int main(){
  test_2();
  return 0;
}
