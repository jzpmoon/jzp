#include "lobj.h"
#include "leval.h"
#include "lparser.h"
#include "lcfun.h"

typedef struct _leval{
  vgc_heap* heap;
  vcontext* ctx;
  ltoken_state* state;
} leval;

static leval eval_instance;

void lstartup(){
  vgc_heap* heap;
  vcontext* ctx;
  ustream* stream;
  ltoken_state* state;

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

  stream = ustream_new(USTREAM_INPUT,USTREAM_FILE);
  if (!stream) {
    uabort("new file input stream error!");
  }

  state = ltoken_state_new(stream,ctx->symtb,ctx->strtb);
  if (!state) {
    uabort("new token state error!");
  }

  lcfun_init(ctx);

  eval_instance.heap = heap;
  eval_instance.ctx = ctx;
  eval_instance.state = state;
  ulog("lstartup");
}

void leval_load(char* file_path){
  FILE* file;
  vps_mod* mod;
  last_obj* ast_obj;

  file = fopen(file_path,"r");
  if(!file){
    uabort("open file error!");
  }
  ltoken_state_reset(eval_instance.state,file);
  ulog1("open file: %s",file_path);
  mod = vps_mod_new();
  if (!mod) {
    uabort("new mod error!");
  }

  while(1){
    ast_obj = lparser_parse(eval_instance.state);
    if (ast_obj == NULL){
      break;
    }
    last2vps(eval_instance.state,ast_obj,mod);
  }

  umem_pool_clean(&eval_instance.state->pool);
  
  vcontext_load(eval_instance.ctx,(vps_t*)mod);
}
