#include "lobj.h"
#include "leval.h"

UDEFUN(UFNAME leval_vps_load,
       UARGS (leval* eval,ustring* name,ustring* path),
       URET vps_mod*)
UDECLARE
  vps_mod* mod;
  vps_prod_ft prod;
UBEGIN
  prod = eval->loader.prod;
  if (prod) {
    mod = prod(eval->src_reader,name,path,&eval->vps);
    vreader_clean(eval->src_reader);
  } else {
    mod = NULL;
  }
  return mod;
UEND

UDEFUN(UFNAME leval_src_load,
       UARGS (leval* eval,char* file_path),
       URET int)
UDECLARE
  vreader* reader;
  ustring* file_path_str;
UBEGIN
  reader = eval->src_reader;
  file_path_str = ustring_table_put(reader->symtb,file_path,-1);
  if (!file_path_str) {
    uabort("file path put symtb error!");
  }
  if (vreader_fi_init_01(reader,file_path_str)) {
    uabort("file infor init error!");
  }
  return 0;
UEND

UDEFUN(UFNAME leval_lib_load,
       UARGS (leval* eval,char* file_path),
       URET int)
UDECLARE
  vreader* reader;
  ustring* file_path_str;
UBEGIN
  reader = eval->lib_reader;
  file_path_str = ustring_table_put(reader->symtb,file_path,-1);
  if (!file_path_str) {
    uabort("file path put symtb error!");
  }
  if (vreader_fi_init_02(reader,eval->self_path,file_path_str)) {
    uabort("file infor init error!");
  }
  return 0;
UEND

UDEFUN(UFNAME leval_conf_load,
       UARGS (leval* eval,char* file_path),
       URET int)
UDECLARE
  vreader* reader;
  ustring* file_path_str;
UBEGIN
  reader = eval->conf_reader;
  file_path_str = ustring_table_put(reader->symtb,file_path,-1);
  if (!file_path_str) {
    uabort("file path put symtb error!");
  }
  if (vreader_fi_init_02(reader,eval->self_path,file_path_str)) {
    uabort("file infor init error!");
  }
  return 0;
UEND

UDEFUN(UFNAME leval_loader_load,
       UARGS (vmod_loader* loader,vmod* mod),
       URET static int)
UDECLARE
  vps_mod* src_mod;
  leval_loader* eval_loader;
  leval* eval;
UBEGIN
  eval_loader = (leval_loader*)loader;
  eval = eval_loader->eval;
  src_mod = leval_vps_load(eval,mod->name,mod->path);
  vcontext_mod2mod(eval->ctx,mod,src_mod);
  vps_cntr_clean(&eval->vps);
  return 0;
UEND

UDEFUN(UFNAME lstartup,
       UARGS (char* self_path,
	      vattr_init_ft attr_init,
	      vattr_init_ft conf_attr_init,
	      lkw_init_ft kw_init,
	      vps_prod_ft prod,
	      vast_attr* symcall),
       URET leval*)
UDECLARE
  leval* eval;
  ustring* self_path_str;
  vgc_heap* heap;
  vcontext* ctx;
  vreader* src_reader;
  vreader* lib_reader;
  vreader* conf_reader;
  leval_loader loader;
UBEGIN
  eval = ualloc(sizeof(leval));
  if (!eval) {
    uabort("new eval error!");
  }
  
  heap = vgc_heap_new(1024*100,
		      1024*10,
		      1024);
  if (!heap) {
    uabort("new heap error!");
  }

  ctx = vcontext_new(heap);
  if (!ctx) {
    uabort("new context error!");
  }

  self_path_str = ustring_table_put(ctx->symtb,self_path,-1);
  if (!self_path_str) {
    uabort("self path new error!");
  }

  src_reader = vreader_new(ctx->symtb,
			   ctx->strtb,
			   attr_init,
			   symcall);
  if (!src_reader) {
    uabort("new source reader error!");
  }

  lib_reader = vreader_new(ctx->symtb,
			   ctx->strtb,
			   attr_init,
			   symcall);
if (!lib_reader) {
    uabort("new library reader error!");
  }

  conf_reader = vreader_new(ctx->symtb,
			    ctx->strtb,
			    conf_attr_init,
			    NULL);
  if (!conf_reader) {
    uabort("new configure reader error!");
  }

  /*keyword init*/
  if (kw_init) {
    kw_init(src_reader);
  }
  if (kw_init) {
    kw_init(lib_reader);
  }

  vps_cntr_init(&eval->vps);

  eval->self_path = self_path_str;
  eval->heap = heap;
  eval->ctx = ctx;
  eval->src_reader = src_reader;
  eval->lib_reader = lib_reader;
  eval->conf_reader = conf_reader;

  loader.load = leval_loader_load;
  loader.eval = eval;
  loader.prod = prod;

  eval->loader = loader;
  
  ctx->loader = (vmod_loader*)&eval->loader;

  return eval;
UEND
