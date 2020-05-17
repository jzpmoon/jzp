#include <stdio.h>
#include "vm.h"
#include "lparser.h"

ltoken_state ts;

int test_1(){
  URI_DEFINE;
  vgc_heap* heap;
  vcontext* ctx;
  FILE* file;
  ustream* stream;
  ltoken_state* ts;
  last_obj* ast_obj;
  vdfg* dfg;

  heap = vgc_heap_new(1024,
		      1024*10,
		      1024);
  if(!heap){
    uabort("new heap error!");
  }

  ctx = vcontext_new(heap);
  if(!ctx){
    uabort("new context error!");
  }
  
  file = fopen("./test.txt","r");
  if(!file){
    uabort("open file error!");
  }
  
  stream = ustream_new_by_file(USTREAM_INPUT,file,URI_REF);
  if(!stream){
    uabort("new stream error!");
  }

  ts = ltoken_state_new(stream,
			ctx->symtb,
			VCONTEXT_SYMTB_SIZE,
			ctx->strtb,
			VCONTEXT_STRTB_SIZE);
  if(!ts){
    uabort("new token state error!");
  }

  ast_obj = lparser_parse(ts);
  last_obj_log(ast_obj);

  dfg = last2dfg(ts,ast_obj);
  
  return 0;
}

int main(){
  test_1();
  return 0;
}
