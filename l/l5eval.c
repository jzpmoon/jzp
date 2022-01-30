#include "lobj.h"
#include "l5eval.h"

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
    } else if (!vast_keywordp(obj)) {
      vast_keyword* kw;

      kw = (vast_keyword*)obj;
      if (kw->kw.kw_type == lkw_true) {
	inst = vps_ipushbool(jreq->vps,grp,vps_bool_true);
      } else if (kw->kw.kw_type == lkw_false) {
	inst = vps_ipushbool(jreq->vps,grp,vps_bool_false);
      } else if (kw->kw.kw_type == lkw_nil) {
	inst = vps_ipushnil(jreq->vps,grp);
      } else {
	uabort("keyword not define!");
	inst = NULL;
      }
      vcfg_grp_inst_apd(grp,inst);
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

static vast_attr lclosure_attr_symcall = {NULL,NULL,symcall_action};

UDEFUN(UFNAME vclosure_cons_call,
       UARGS (vclosure* closure,vps_closure_req* req,vast_obj* cons),
       URET static void)
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
  if (!vast_integerp(car)) {
    vast_integer* inte = (vast_integer*)car;
    inst = vps_ipushint(req->vps,grp,inte->name,inte->value);
    vcfg_grp_inst_apd(grp,inst);
  } else if (!vast_numberp(car)) {
    vast_number* num = (vast_number*)car;
    inst = vps_ipushnum(req->vps,grp,num->name,num->value);
    vcfg_grp_inst_apd(grp,inst);
  } else if (!vast_symbolp(car)) {
    sym = (vast_symbol*)car;
    data = vclosure_field_get(closure,sym->name);
    if (data) {
      if (data->scope == VPS_SCOPE_LOCAL) {
	inst = vps_iloaddt(req->vps,sym->name);
	vcfg_grp_inst_apd(grp,inst);
      } else if (data->scope == VPS_SCOPE_GLOBAL) {
	inst = vps_ipushdt(req->vps,grp,sym->name);
	vcfg_grp_inst_apd(grp,inst);
      } else if (data->scope == VPS_SCOPE_DECL) {
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
  } else if (!vast_keywordp(car)) {
    vast_keyword* kw;
    
    kw = (vast_keyword*)car;
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
    uabort("car not a number or symbol!");
  }
UEND

UDEFUN(UFNAME vclosure_atom_call,
       UARGS (vclosure* closure,vps_cntr* vps,vast_obj* obj),
       URET static void)
UDECLARE
  vps_inst* inst;
  vcfg_graph* grp;
UBEGIN
  grp = closure->init;
  if (!vast_numberp(obj)) {
    vast_number* num = (vast_number*)obj;
    inst = vps_ipushnum(vps,grp,num->name,num->value);
  } else if (!vast_integerp(obj)) {
     vast_integer* inte = (vast_integer*)obj;
     inst = vps_ipushint(vps,grp,inte->name,inte->value);
  } else if (!vast_symbolp(obj)) {
    vast_symbol* sym = (vast_symbol*)obj;
    vps_data* data = vclosure_field_get(closure,sym->name);
    if (!data) {
      uabort("variable:%s,not define!",sym->name->value);
    }
    if (data->scope == VPS_SCOPE_LOCAL) {
      inst = vps_iloaddt(vps,sym->name);
    } else if (data->scope == VPS_SCOPE_GLOBAL) {
      inst = vps_ipushdt(vps,grp,sym->name);
    } else if (data->scope == VPS_SCOPE_DECL) {
      inst = vps_ipushdt(vps,grp,sym->name);
    } else {
      uabort("variable:%s,scope error!",sym->name->value);
      inst = NULL;
    }
  } else if (!vast_keywordp(obj)) {
    vast_keyword* kw;
    
    kw = (vast_keyword*)obj;
    if (kw->kw.kw_type == lkw_true) {
      inst = vps_ipushbool(vps,grp,vps_bool_true);
    } else if (kw->kw.kw_type == lkw_false) {
      inst = vps_ipushbool(vps,grp,vps_bool_false);
    } else if (kw->kw.kw_type == lkw_nil) {
      inst = vps_ipushnil(vps,grp);
    } else {
      uabort("keyword not define!");
      inst = NULL;
    }
  } else {
    uabort("args not a number or symbol!");
    inst = NULL;
  }
  vcfg_grp_inst_apd(grp,inst);
UEND

UDEFUN(UFNAME vclosure_arg_call,
       UARGS (vclosure* closure,vps_closure_req* req,vast_obj* obj),
       URET static void)
UDECLARE

UBEGIN
  if (!vast_consp(obj)) {
    vclosure_cons_call(req->closure,req,obj);
  } else {
    vclosure_atom_call(req->closure,req->vps,obj);
  }
UEND

#include "_l5temp.attr"
#include "_l5temp.cfun"

static void l5cfun_init(vcontext* ctx,vmod* mod)
{
  l5cfun_file_concat_init(ctx,mod);
}

static void l5attr_init(vreader* reader)
{
  l5attr_file_concat_init(reader);
}

UDEFUN(UFNAME l5kw_init,
       UARGS (vreader* reader),
       URET static void)
UDECLARE

UBEGIN
  /*keyword init*/
  #define DF(no,str)				       \
  if (vreader_keyword_put(reader,(vast_kw){no,str})) { \
    uabort("keyword put error!");		       \
  }
  #include "l5kw.h"
  #undef DF
UEND

UDEFUN(UFNAME l5startup,
       UARGS (),
       URET leval*)
UDECLARE
  leval* eval;
UBEGIN
  eval = lstartup(l5attr_init,
		  l5cfun_init,
		  l5kw_init,
		  vclosure2vps,
		  &lclosure_attr_symcall);
  
  return eval;
UEND
