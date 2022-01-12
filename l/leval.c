#include "lobj.h"
#include "leval.h"

vps_mod* leval_vps_load(leval* eval,char* file_path)
{
  vps_mod* mod;
  vps_prod_ft prod;

  prod = eval->loader.prod;
  if (prod) {
    mod = prod(eval->reader,file_path,&eval->vps);
    vreader_clean(eval->reader);
  } else {
    mod = NULL;
  }
  return mod;
}

int leval_load(leval* eval,char* file_path)
{
  vps_mod* mod;
  vps_cntr* vps;
  
  mod = leval_vps_load(eval,file_path);
  vps = &eval->vps;
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

UDEFUN(UFNAME leval_kw_init,
       UARGS (vreader* reader),
       URET static void)
UDECLARE

UBEGIN
  /*keyword init*/
  #define DF(no,str)				       \
  if (vreader_keyword_put(reader,(vast_kw){no,str})) { \
    uabort("keyword put error!");		       \
  }
  #include "lkw.h"
  #undef DF
UEND

UDEFUN(UFNAME lstartup,
       UARGS (vattr_init_ft attr_init,
	      vcfun_init_ft cfun_init,
	      vps_prod_ft prod,
	      vast_attr* symcall),
       URET leval*)
UDECLARE
  leval* eval;
  vgc_heap* heap;
  vcontext* ctx;
  vmod* mod;
  ustring* mod_name;
  vreader* reader;
  leval_loader loader;
UBEGIN
  eval = ualloc(sizeof(leval));
  if (!eval) {
    uabort("new eval error!");
  }
  
  heap = vgc_heap_new(1024*100,
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

  reader = vreader_new(ctx->symtb,
		       ctx->strtb,
		       attr_init,
		       symcall);
  if (!reader) {
    uabort("new reader error!");
  }

  /*keyword init*/
  leval_kw_init(reader);

  vps_cntr_init(&eval->vps);

  eval->heap = heap;
  eval->ctx = ctx;
  eval->mod = mod;
  eval->reader = reader;
  
  loader.load = leval_loader_load;
  loader.eval = eval;
  loader.prod = prod;

  eval->loader = loader;
  
  cfun_init(ctx,eval->mod);
  vmod_loaded(mod);
  
  ctx->loader = (vmod_loader*)&eval->loader;

  return eval;
UEND
