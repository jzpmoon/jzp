#include "lobj.h"
#include "leval.h"
#include "lcfun.h"

vps_mod* leval_vps_load(leval* eval,char* file_path)
{
  FILE* file;
  vps_mod* mod;
  last_obj* ast_obj;
  ustring* mod_name;
  
  file = fopen(file_path,"r");
  if(!file){
    uabort("open file error!");
  }
  ltoken_state_reset(eval->ts,file);

  mod_name = ustring_table_put(eval->ctx->symtb,file_path,-1);
  if (!mod_name) {
    uabort("mod name put symtb error!");
  }

  mod = vps_mod_new(&eval->vps,mod_name);
  if (!mod) {
    uabort("new mod error!");
  }

  while(1){
    ast_obj = lparser_parse(eval->ts);
    if (ast_obj == NULL){
      break;
    }
    last2vps(eval->ts,ast_obj,mod);
  }

  vps_mod_loaded(mod);
  umem_pool_clean(&eval->ts->pool);
  
  return mod;
}

int leval_load(leval* eval,char* file_path)
{
  vps_mod* mod;
  vps_cntr* vps;
  
  mod = leval_vps_load(eval,file_path);
  vps = &eval->vps;
  vps_cntr_load(vps,mod);
  vcontext_vps_load(eval->ctx,vps);
  return 0;
}

static int leval_loader_load(vmod_loader* loader,vmod* mod){
  vmod* dest_mod;
  vps_mod* src_mod;
  char* file_path;
  leval_loader* eval_loader;
  leval* eval;

  dest_mod = mod;
  file_path = mod->name->value;
  eval_loader = (leval_loader*)loader;
  eval = eval_loader->eval;
  src_mod = leval_vps_load(eval,file_path);
  vcontext_mod2mod(eval->ctx,dest_mod,src_mod);
  vps_cntr_clean(&eval->vps);
  ulog("leval_loader_load");
  return 0;
}

leval* lstartup(){
  leval* eval;
  vgc_heap* heap;
  vcontext* ctx;
  vmod* mod;
  ustring* mod_name;
  ustream* stream;
  ltoken_state* ts;
  leval_loader loader;

  eval = ualloc(sizeof(leval));
  if (!eval) {
    uabort("new eval error!");
  }
  
  heap = vgc_heap_new(1024*10,
		      1024*10,
		      1024);
  if(!heap){
    uabort("new heap error!");
  }

  ctx = vcontext_new(heap);
  if(!ctx){
    uabort("new context error!");
  }
  
  mod_name = ustring_table_put(ctx->symtb,"sysmod",-1);
  if (!mod_name) {
    uabort("mod name put symtb error!");
  }
  
  mod = vcontext_mod_add(ctx,mod_name);

  stream = ustream_new(USTREAM_INPUT,USTREAM_FILE);
  if (!stream) {
    uabort("new file input stream error!");
  }

  ts = ltoken_state_new(stream,ctx->symtb,ctx->strtb);
  if (!ts) {
    uabort("new token state error!");
  }

  vps_cntr_init(&eval->vps);

  eval->heap = heap;
  eval->ctx = ctx;
  eval->mod = mod;
  eval->ts = ts;
  
  loader.load = leval_loader_load;
  loader.eval = eval;

  eval->loader = loader;
  
  lcfun_init(ctx,eval->mod);
  vmod_loaded(mod);
  
  ctx->loader = (vmod_loader*)&eval->loader;

  ulog("lstartup");
  
  return eval;
}
