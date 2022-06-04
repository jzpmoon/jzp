#include "lobj.h"
#include "l5eval.h"
#include "l5.conf.attr"

static void vcall l5conf_init(vreader* reader)
{
  _vattr_file_init_l5_conf(reader);
}

UDEFUN(UFNAME vclosure_arg_call,
       UARGS (vclosure* closure,vps_closure_req* req,vast_obj* obj),
       URET static void)
UDECLARE
  vps_closure_req dreq;
  vast_attr_res dres;
  vps_cntr* vps;
  vcfg_graph* grp;
  vps_inst* inst;
UBEGIN
  vps = req->vps;
  grp = closure->init;
  if (obj->t == vastk_symbol) {
    vast_symbol* sym = (vast_symbol*)obj;
    vps_data* data = vclosure_field_get(closure,sym->name);
    if (data) {
      if (data->scope == VPS_SCOPE_LOCAL) {
	inst = vps_iloaddt(vps,sym->name);
	vcfg_grp_inst_apd(grp,inst);
      } else if (data->scope == VPS_SCOPE_GLOBAL) {
	inst = vps_ipushdt(vps,grp,sym->name);
	vcfg_grp_inst_apd(grp,inst);
      } else if (data->scope == VPS_SCOPE_DECL) {
	inst = vps_ipushdt(vps,grp,sym->name);
	vcfg_grp_inst_apd(grp,inst);
      } else {
	uabort("variable:%s,scope error!",sym->name->value);
      }
    } else {
      uabort("symbol:%s not a variable!",sym->name->value);
    }
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
  } else if(obj->t == vastk_character) {
    vast_character* chara = (vast_character*)obj;
    inst = vps_ipushchar(vps,grp,chara->value);
    vcfg_grp_inst_apd(grp,inst);
  } else if (!vast_consp(obj)) {
    vast_obj* car = vast_car(obj);
    if (!vast_symbolp(car)) {
      vast_symbol* sym = (vast_symbol*)car;
      vast_attr* attr = sym->attr;
      if (attr) {
	dreq = vast_req_dbl(req);
	dreq.ast_obj = obj;
	dreq.closure = closure;
	vast_attr_call(attr,(vast_attr_req*)&dreq,&dres);
      } else {
	uabort("symbol has no attr!");
      }
    } else {
      uabort("car not a symbol!");
    }
  } else if (!vast_keywordp(obj)) {
    vast_keyword* kw;

    kw = (vast_keyword*)obj;
    if (kw->kw.kw_type == lkw_true) {
      inst = vps_ipushbool(req->vps,grp,vps_bool_true);
    } else if (kw->kw.kw_type == lkw_false) {
      inst = vps_ipushbool(req->vps,grp,vps_bool_false);
    } else if (kw->kw.kw_type == lkw_nil) {
      inst = vps_ipushnil(req->vps,grp);
    } else {
      uabort("keyword not define!");
      inst = NULL;
    }
    vcfg_grp_inst_apd(grp,inst);
  } else {
    uabort("push inst error!");
  }
UEND

UDEFUN(UFNAME symcall_action,
       UARGS (vast_attr_req* req,
	      vast_attr_res* res),
       URET static int vcall)
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
    vclosure_arg_call(closure,jreq,obj);
    next = vast_cdr(next);
  }
  /* push subr name */
  inst = vps_ipushdt(vps,grp,symbol->name);
  vcfg_grp_inst_apd(grp,inst);
  /* call subr */
  inst = vps_icall(vps);
  vcfg_grp_inst_apd(grp,inst);

  return 0;
UEND

static vast_attr lclosure_attr_symcall = {NULL,NULL,symcall_action};

#include "_l5temp.attr"

static void vcall l5attr_init(vreader* reader)
{
  l5attr_file_concat_init(reader);
}

UDEFUN(UFNAME l5kw_init,
       UARGS (vreader* reader),
       URET static void)
UDECLARE
  vast_kw kw;
UBEGIN
  /*keyword init*/
  #define DF(no,str)				       \
  kw.kw_type = no;                                     \
  kw.kw_str = str;                                     \
  if (vreader_keyword_put(reader,kw)) {                \
    uabort("keyword put error!");		       \
  }
  #include "l5kw.h"
  #undef DF
UEND

UDEFUN(UFNAME l5startup,
       UARGS (char* self_path,
	      int vm_gc_asz,
	      int vm_gc_ssz,
	      int vm_gc_rsz),
       URET l5api l5eval* l5call)
UDECLARE
  l5eval* eval;
  leval* base_eval;
  vclosure* top_closure;
UBEGIN
  base_eval = lstartup(self_path,
		       vm_gc_asz,
		       vm_gc_ssz,
		       vm_gc_rsz,
		       l5attr_init,
		       l5conf_init,
		       l5kw_init,
		       vclosure2vps,
		       &lclosure_attr_symcall);
  if (!base_eval) {
    uabort("startup base eval error!");
  }
  top_closure = vclosure_new(&base_eval->vps);
  if (!top_closure) {
    uabort("top closure new error!");
  }
  eval = ualloc(sizeof(l5eval));
  if (!eval) {
    uabort("new eval error!");
  }
  eval->base_eval = base_eval;
  eval->top_closure = top_closure;
  return eval;
UEND

UDEFUN(UFNAME l5eval_src_load,
       UARGS (l5eval* eval,char* file_path),
       URET l5api int l5call)
UDECLARE
  vreader* reader;
  leval* base_eval;
UBEGIN
  base_eval = eval->base_eval;
  reader = base_eval->src_reader;
  leval_src_load(base_eval,file_path);
  vfile2closure(eval->top_closure,
	        reader,
		reader->fi.file_name,
	        reader->fi.file_path,
		&base_eval->vps,
		VCLOSURE_TYPE_MAIN);
  vclosure2mod(eval->top_closure,&base_eval->vps,NULL);
  vreader_clean(reader);
  vcontext_vps_load(base_eval->ctx,&base_eval->vps);
  return 0;
UEND

UDEFUN(UFNAME l5eval_lib_load,
       UARGS (l5eval* eval,char* file_path),
       URET l5api int l5call)
UDECLARE
  vreader* reader;
  leval* base_eval;
UBEGIN
  base_eval = eval->base_eval;
  reader = base_eval->lib_reader;
  leval_lib_load(base_eval,file_path);
  vfile2closure(eval->top_closure,
		reader,
		reader->fi.file_name,
		reader->fi.file_path,
		&base_eval->vps,
		VCLOSURE_TYPE_FILE);
  vreader_clean(reader);
  return 0;
UEND

UDEFUN(UFNAME l5eval_conf_load,
       UARGS (l5eval* eval,char* file_path),
       URET l5api int l5call)
UDECLARE
  vreader* reader;
  vast_conf_req req;
  vast_attr_res res;
UBEGIN
  reader = eval->base_eval->conf_reader;
  leval_conf_load(eval->base_eval,file_path);
  req.reader = reader;
  req.eval = (leval*)eval;
  if (vfile2obj(reader,reader->fi.file_path,(vast_attr_req*)&req,&res)) {
    uabort("file2obj error!");
  }
  return 0;
UEND
