#include "lobj.h"
#include "leval.h"

UDEFUN(UFNAME leval_vps_load,
       UARGS (leval* eval,char* file_path),
       URET vps_mod*)
UDECLARE
  vps_mod* mod;
  vps_prod_ft prod;
UBEGIN
  prod = eval->loader.prod;
  if (prod) {
    mod = prod(eval->reader,file_path,&eval->vps);
    vreader_clean(eval->reader);
  } else {
    mod = NULL;
  }
  return mod;
UEND

UDEFUN(UFNAME leval_load,
       UARGS (leval* eval,char* file_path),
       URET int)
UDECLARE
  vreader* reader;
  ustring* file_path_str;
  vps_mod* mod;
  vps_cntr* vps;
UBEGIN
  reader = eval->reader;
  file_path_str = ustring_table_put(reader->symtb,file_path,-1);
  if (!file_path_str) {
    uabort("file path put symtb error!");
  }
  if (!ufile_init_by_strtb(reader->symtb,&reader->fi,file_path_str)) {
    uabort("file infor init error!");
  }
  mod = leval_vps_load(eval,file_path);
  vps = &eval->vps;
  vcontext_vps_load(eval->ctx,vps);
  return 0;
UEND

UDEFUN(UFNAME leval_loader_load,
       UARGS (vmod_loader* loader,vmod* mod),
       URET static int)
UDECLARE
  vmod* dest_mod;
  vps_mod* src_mod;
  char* file_path;
  leval_loader* eval_loader;
  leval* eval;
UBEGIN
  dest_mod = mod;
  file_path = mod->name->value;
  eval_loader = (leval_loader*)loader;
  eval = eval_loader->eval;
  src_mod = leval_vps_load(eval,file_path);
  vcontext_mod2mod(eval->ctx,dest_mod,src_mod);
  vps_cntr_clean(&eval->vps);
  return 0;
UEND

UDEFUN(UFNAME lstartup,
       UARGS (vattr_init_ft attr_init,
	      vcfun_init_ft cfun_init,
	      lkw_init_ft kw_init,
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
  if (kw_init) {
    kw_init(reader);
  }

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
