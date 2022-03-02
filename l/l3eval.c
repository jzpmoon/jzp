#include "lobj.h"
#include "l3eval.h"
#include "l3.conf.attr"

static void l3conf_init(vreader* reader)
{
  _vattr_file_init_l3_conf(reader);
}

UDEFUN(UFNAME symcall_action,
       UARGS (vast_attr_req* req,
	      vast_attr_res* res),
       URET static int)
UDECLARE
  vast_obj* ast_obj;
  vps_jzp_req* jreq;
  vps_cntr* vps;
  vps_cfg* parent;
  vcfg_graph* grp;
  vps_inst* inst;
  vast_obj* obj;
  vast_obj* next;
  vast_symbol* symbol;
UBEGIN
  ast_obj = req->ast_obj;
  jreq = (vps_jzp_req*)req;
  vps = jreq->vps;
  parent = jreq->parent;
  
  if (parent->t != vcfgk_grp) {
    uabort("parent not a graph!");
  }
  grp = (vcfg_graph*)parent;
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

  ulog0("symcall");
  VATTR_RETURN_VOID;
UEND

static vast_attr last_attr_symcall = {NULL,NULL,symcall_action};

#include "_l3temp.attr"
#include "_l3temp.cfun"

static void l3cfun_init(vcontext* ctx,vmod* mod)
{
  l3cfun_file_concat_init(ctx,mod);
}

static void l3attr_init(vreader* reader)
{
  l3attr_file_concat_init(reader);
}

UDEFUN(UFNAME l3kw_init,
       UARGS (vreader* reader),
       URET static void)
UDECLARE

UBEGIN
  /*keyword init*/
  #define DF(no,str)				       \
  if (vreader_keyword_put(reader,(vast_kw){no,str})) { \
    uabort("keyword put error!");		       \
  }
  #include "l3kw.h"
  #undef DF
UEND
  
UDEFUN(UFNAME l3startup,
       UARGS (),
       URET leval*)
UDECLARE
  leval* eval;
UBEGIN
  eval = lstartup(l3attr_init,
		  l3conf_init,
		  l3cfun_init,
		  l3kw_init,
		  vfile2vps,
		  &last_attr_symcall);
  
  return eval;
UEND
