#include "ulist_tpl.c"
#include "vparser.h"
#include "vclosure.h"

ulist_def_tpl(vclosurep);

UDEFUN(UFNAME vclosure_new,
       UARGS (vps_cntr* vps),
       URET vclosure*)
UDECLARE
  vclosure* closure;
  uallocator* allocator;
UBEGIN
  allocator = vps_cntr_alloc_get(vps);
  closure = allocator->alloc(allocator,sizeof(vclosure));
  if (closure) {
    closure->fields = ulist_vps_datap_alloc(allocator);
    if (!closure->fields) {
      goto err;
    }
    closure->init = NULL;
    closure->parent = NULL;
    closure->childs = ulist_vclosurep_alloc(allocator);
    if (!closure->childs) {
      goto err;
    }
  }

  return closure;
 err:
  allocator->clean(allocator);
  return NULL;
UEND

UDEFUN(UFNAME vclosure2vps,
       UARGS (vreader* reader,char* file_path,vps_cntr* vps),
       URET vps_mod*)
UDECLARE
  FILE* file;
  vps_mod* mod;
  ustring* mod_name;
  vclosure* closure;
  vps_closure_req req;
  vast_attr_res res;
UBEGIN
  mod_name = ustring_table_put(reader->symtb,file_path,-1);
  if (!mod_name) {
    uabort("mod name put symtb error!");
  }

  mod = vps_mod_new(vps,mod_name);
  if (!mod) {
    uabort("new mod error!");
  }
  vps_cntr_load(vps,mod);
  
  file = fopen(file_path,"r");
  if(!file){
    uabort("open file error!");
  }
  closure = vclosure_new(vps);
  if (!closure) {
    uabort("closure new error!");
  }
  req.vps = vps;
  req.closure = closure;
  req.reader = reader;
  if (vfile2obj(reader,file_path,(vast_attr_req*)&req,&res)) {
    uabort("file2obj error!");
  }
  vps_mod_loaded(mod);

  return mod;
UEND

UDEFUN(UFNAME symcall_action,
       UARGS (vast_attr_req* req,
	      vast_attr_res* res),
       URET static int)
UDECLARE

UBEGIN

  VATTR_RETURN_VOID;
UEND

vast_attr vclosure_attr_symcall = {NULL,NULL,symcall_action};
