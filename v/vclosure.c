#include "ulist_tpl.c"
#include "vparser.h"
#include "vclosure.h"

ulist_def_tpl(vclosurep);

UDEFUN(UFNAME vclosure_new,
       UARGS (vps_cntr* vps),
       URET vclosure*)
UDECLARE
  uallocator* allocator;
  vclosure* closure;
  ulist_vps_datap* fields;
  vcfg_graph* init;
  ulist_vclosurep* childs;
UBEGIN
  allocator = vps_cntr_alloc_get(vps);
  closure = allocator->alloc(allocator,sizeof(vclosure));
  if (closure) {
    fields = ulist_vps_datap_alloc(allocator);
    if (!closure->fields) {
      goto err;
    }
    init = vcfg_graph_new(vps,NULL);
    if (!init) {
      goto err;
    }
    childs = ulist_vclosurep_alloc(allocator);
    if (!closure->childs) {
      goto err;
    }
    closure->fields = fields;
    closure->init = init;
    closure->parent = NULL;
    closure->childs = childs;
  }

  return closure;
 err:
  allocator->clean(allocator);
  return NULL;
UEND

UDEFUN(UFNAME vchilds2mod,
       UARGS (ulist_vclosurep* childs,vps_mod* mod),
       URET static void)
UDECLARE
  uset* set;
  ucursor c;
UBEGIN
  set = (uset*)childs;
  set->iterate(&c);
  while (1) {
    vclosurep* childp = set->next(set,&c);
    vclosure* child;
    if (!childp) {
      break;
    }
    child = *childp;
    vps_mod_code_put(mod,child->init);
    /*childs*/
    if (child->childs) {
      vchilds2mod(child->childs,mod);
    }
  }
UEND

UDEFUN(UFNAME vclosure2mod,
       UARGS (vclosure* closure,vps_mod* mod),
       URET static void)
UDECLARE
  uset* set;
  ucursor c;
UBEGIN
  /*fields*/
  set = (uset*)closure->fields;
  set->iterate(&c);
  while (1) {
    vps_datap* fieldp = set->next(set,&c);
    if (!fieldp) {
      break;
    }
    vps_mod_data_put(mod,*fieldp);
  }
  /*entry*/
  mod->entry = closure->init;
  /*childs*/
  if (closure->childs) {
    vchilds2mod(closure->childs,mod);
  }
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

  vclosure2mod(closure,mod);

  vps_mod_loaded(mod);

  return mod;
UEND

UDEFUN(UFNAME symcall_action,
       UARGS (vast_attr_req* req,
	      vast_attr_res* res),
       URET static int)
UDECLARE
  vast_obj* ast_obj;
  vps_closure_req* jreq;
  vps_cntr* vps;
  vclosure* closure;
  vcfg_graph* grp;
  vps_inst* inst;
  vast_obj* obj;
  vast_obj* next;
  vast_symbol* symbol;
UBEGIN
  jreq = (vps_closure_req*)req;
  ast_obj = jreq->ast_obj;
  vps = jreq->vps;
  closure = jreq->closure;
  grp = closure->init;
  obj = vast_car(ast_obj);
  symbol = (vast_symbol*)obj;
  /* push params */
  next = vast_cdr(ast_obj);
  while (next) {
    obj = vast_car(next);
    if (obj->t == vastk_symbol) {
      vast_symbol* sym = (vast_symbol*)obj;
      inst = vps_ipushdt(vps,grp,sym->name);
    }else if(obj->t == vastk_integer){
      vast_integer* inte = (vast_integer*)obj;
      inst = vps_ipushint(vps,grp,inte->name,inte->value);
    }else if(obj->t == vastk_number){
      vast_number* num = (vast_number*)obj;
      inst = vps_ipushnum(vps,grp,num->name,num->value);
    }else if(obj->t == vastk_string){
      vast_string* str = (vast_string*)obj;
      inst = vps_ipushstr(vps,grp,str->value);
    }else{
      uabort("push inst error!");
      inst = NULL;
    }
    vcfg_grp_inst_apd(grp,inst);
    next = vast_cdr(next);
  }
  /* push subr name */
  inst = vps_ipushdt(vps,grp,symbol->name);
  vcfg_grp_inst_apd(grp,inst);
  /* call subr */
  inst = vps_icall(vps);
  vcfg_grp_inst_apd(grp,inst);

  VATTR_RETURN_VOID;
UEND

vast_attr vclosure_attr_symcall = {NULL,NULL,symcall_action};

UDEFUN(UFNAME vclosure_child_add,
	 UARGS (vclosure* closure,vclosure* child),
	 URET int)
UDECLARE

UBEGIN
  return ulist_vclosurep_append(closure->childs,child);
UEND
