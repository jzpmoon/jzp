#include <stdio.h>
#include "vm.h"
#include "lparser.h"
#include "lcfun.h"
#include "leval.h"

ltoken_state ts;

int test_1(){
  URI_DEFINE;
  vgc_heap* heap;
  vcontext* ctx;
  FILE* file;
  ustream* stream;
  ltoken_state* ts;
  last_obj* ast_obj;
  vps_mod* mod;

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
			ctx->strtb);
  if(!ts){
    uabort("new token state error!");
  }

  lcfun_init(ctx);
  
  mod = vps_mod_new();
  if (!mod) {
    uabort("new mod error!");
  }
  
  while(1){
    ast_obj = lparser_parse(ts);
    if (ast_obj == NULL){
      break;
    }
    /*last_obj_log(ast_obj);*/
    last2vps(ts,ast_obj,mod);
  }
  
  vcontext_load(ctx,(vps_t*)mod);

  return 0;
}

int test_2(){
  lstartup();
  leval_load("./test.txt");
  return 0;
}

int main(){
  test_2();
  return 0;
}
