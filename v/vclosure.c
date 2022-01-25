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
    if (!fields) {
      goto err;
    }
    init = vcfg_graph_new(vps,NULL);
    if (!init) {
      goto err;
    }
    childs = ulist_vclosurep_alloc(allocator);
    if (!childs) {
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
  vcfg_graph* init;
  vps_inst* inst;
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
  init = closure->init;
  init->scope = VPS_SCOPE_ENTRY;
  req.vps = vps;
  req.closure = closure;
  req.reader = reader;
  if (vfile2obj(reader,file_path,(vast_attr_req*)&req,&res)) {
    uabort("file2obj error!");
  }

  inst = vps_iretvoid(vps);
  vcfg_grp_inst_apd(init,inst);
  vcfg_grp_build(vps,init);
  vcfg_grp_connect(vps,init);

  vclosure2mod(closure,mod);

  vps_mod_loaded(mod);

  return mod;
UEND

UDEFUN(UFNAME vclosure_field_get,
       UARGS (vclosure* closure,ustring* name),
       URET vps_data*)
UDECLARE
  uset* fields;
  ucursor c;
  vps_data* data;
UBEGIN
  /*find local fileds*/
  data = vcfg_grp_dtget(closure->init,name);
  if (data) {
    return data;
  }

  /*find closure fields*/
  fields = (uset*)closure->fields;
  fields->iterate(&c);
  while (1) {
    vps_datap* dp = fields->next(fields,&c);
    if (!dp) {
      break;
    }
    data = *dp;
    if (ustring_comp(name,data->name)) {
      return data;
    }
  }

  /*find parent fields*/
  if (closure->parent) {
    return vclosure_field_get(closure->parent,name);
  } else {
    return NULL;
  }
UEND

UDEFUN(UFNAME symcall_action,
       UARGS (vast_attr_req* req,
	      vast_attr_res* res),
       URET static int)
UDECLARE
  vast_obj* ast_obj;
  vps_closure_req* jreq;
  vps_closure_req dreq;
  vast_attr_res dres;
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
      vcfg_grp_inst_apd(grp,inst);
    } else if (obj->t == vastk_integer) {
      vast_integer* inte = (vast_integer*)obj;
      inst = vps_ipushint(vps,grp,inte->name,inte->value);
      vcfg_grp_inst_apd(grp,inst);
    } else if (obj->t == vastk_number) {
      vast_number* num = (vast_number*)obj;
      inst = vps_ipushnum(vps,grp,num->name,num->value);
      vcfg_grp_inst_apd(grp,inst);
    } else if (obj->t == vastk_string) {
      vast_string* str = (vast_string*)obj;
      inst = vps_ipushstr(vps,grp,str->value);
      vcfg_grp_inst_apd(grp,inst);
    } else if (!vast_consp(obj)) {
      vast_obj* car = vast_car(obj);
      if (!vast_symbolp(car)) {
	vast_symbol* sym = (vast_symbol*)car;
	vast_attr* attr = sym->attr;
	if (attr) {
	  dreq = vast_req_dbl(jreq);
	  dreq.ast_obj = obj;
	  attr->action((vast_attr_req*)&dreq,&dres);
	} else {
	  uabort("symbol has no attr!");
	}
      } else {
	uabort("car not a symbol!");
      }
    } else {
      uabort("push inst error!");
    }
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

UDEFUN(UFNAME vclosure_cons_call,
       UARGS (vclosure* closure,vps_closure_req* req,vast_obj* cons),
       URET void)
UDECLARE
  vps_closure_req dreq;
  vast_attr_res dres;
  vast_obj* car;
  vast_symbol* sym;
  vps_data* data;
  vps_inst* inst;
  vcfg_graph* grp;
  vast_attr* attr;
UBEGIN
  grp = closure->init;
  car = vast_car(cons);
  if (vast_symbolp(car)) {
    uabort("cons car not a symbol!");
  }
  sym = (vast_symbol*)car;
  data = vclosure_field_get(closure,sym->name);
  if (data) {
    if (data->scope == VPS_SCOPE_LOCAL) {
      inst = vps_iloaddt(req->vps,sym->name);
      vcfg_grp_inst_apd(grp,inst);
    } else if (data->scope == VPS_SCOPE_GLOBAL) {
      inst = vps_ipushdt(req->vps,grp,sym->name);
      vcfg_grp_inst_apd(grp,inst);
    } else {
      uabort("variable:%s,scope error!",sym->name->value);
    }
  } else {
    dreq = vast_req_dbl(req);
    dreq.ast_obj = cons;
    dreq.closure = closure;
    attr = sym->attr;
    if (attr) {
      attr->action((vast_attr_req*)&dreq,&dres);
    } else {
      uabort("cons car symbol has no attr!");
    }
  }
 UEND
