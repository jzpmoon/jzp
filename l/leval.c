#include "lobj.h"
#include "leval.h"
#include "lparser.h"
#include "lcfun.h"

typedef struct _leval{
  vgc_heap* heap;
  vcontext* ctx;
  ltoken_state* ts;
  vps_cntr vps;
} leval;

static leval eval_instance;

void lstartup(){
  vgc_heap* heap;
  vcontext* ctx;
  ustream* stream;
  ltoken_state* ts;

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

  ts = ltoken_state_new(stream,ctx->symtb,ctx->strtb);
  if (!ts) {
    uabort("new token state error!");
  }

  vps_cntr_init(&eval_instance.vps);

  lcfun_init(ctx);

  eval_instance.heap = heap;
  eval_instance.ctx = ctx;
  eval_instance.ts = ts;
  ulog("lstartup");
}

void leval_load(char* file_path){
  FILE* file;
  vps_mod* mod;
  last_obj* ast_obj;
  ustring* mod_name;
  
  file = fopen(file_path,"r");
  if(!file){
    uabort("open file error!");
  }
  ltoken_state_reset(eval_instance.ts,file);

  mod_name = ustring_table_put(eval_instance.ctx->symtb,file_path,-1);
  if (!mod_name) {
    uabort("mod name put symtb error!");
  }

  mod = vps_mod_new(&eval_instance.vps,mod_name);
  if (!mod) {
    uabort("new mod error!");
  }

  vps_cntr_load(&eval_instance.vps,mod);

  while(1){
    ast_obj = lparser_parse(eval_instance.ts);
    if (ast_obj == NULL){
      break;
    }
    last2vps(eval_instance.ts,ast_obj,mod);
  }

  umem_pool_clean(&eval_instance.ts->pool);
  
  vcontext_load(eval_instance.ctx,(vps_t*)mod);
}
