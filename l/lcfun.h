#ifndef _LCFUN_H_
#define _LCFUN_H_

#include "vcontext.h"

struct _lcfun_infor{
  char* lname;
  ustring* name;
  int params_count;
  int has_retval;
  int scope;
  vcfun_ft entry;
};

typedef vgc_obj*(*lvar_gen_ft)();

struct _lvar_infor{
  char* lname;
  ustring* name;
  int scope;
  lvar_gen_ft var_gen;
};

#define LSCOPE_GLOBAL VPS_SCOPE_GLOBAL
#define LSCOPE_LOCAL VPS_SCOPE_LOCAL
#define LRETVAL_YES 1
#define LRETVAL_NO 0

#define LDEFUN(FNAME,LNAME,SCOPE,PCOUNT,RETVAL,BODY)	\
  int _lcfun_entry_##FNAME(vcontext* ctx){		\
    BODY;						\
  }							\
  static struct _lcfun_infor _lcfun_infor_##FNAME =	\
    {LNAME,NULL,PCOUNT,RETVAL,SCOPE,_lcfun_entry_##FNAME};

#define LDEFVAR(FNAME,LNAME,SCOPE,BODY)			\
  vgc_obj* _lvar_gen_##FNAME(vcontext* ctx){		\
    BODY;						\
  }							\
  static struct _lvar_infor _lvar_infor_##FNAME =	\
    {LNAME,NULL,SCOPE,_lvar_gen_##FNAME};  

#define LFUNONLOAD(fname,body)					\
  void _lcfun_file_init_##fname(vcontext* ctx,vmod* mod){	\
    body							\
  }

#define LDECLFUN(NAME) LFUN_INIT(ctx,mod,NAME)

#define LFUN_INIT(CTX,MOD,FNAME)				\
  do{								\
    vgc_cfun* cfun;						\
    ustring* str;						\
    str = ustring_table_put((CTX)->symtb,			\
			    _lcfun_infor_##FNAME.lname,		\
			    -1);				\
    if(!str){uabort("init cfun error!");}			\
    _lcfun_infor_##FNAME.name = str;				\
    cfun = vgc_cfun_new((CTX)->heap,				\
			_lcfun_infor_##FNAME.entry,		\
			_lcfun_infor_##FNAME.params_count,	\
      			_lcfun_infor_##FNAME.has_retval,	\
			vgc_heap_area_static);			\
    if(!cfun){							\
      uabort("LFUN_INIT:cfun new error!");			\
    }								\
    vmod_lobj_put((CTX)->heap,					\
		  MOD,						\
		  _lcfun_infor_##FNAME.name,			\
		  (vgc_obj*)cfun);				\
    if (_lcfun_infor_##FNAME.scope == VPS_SCOPE_GLOBAL) {	\
      vmod_gobj_put((CTX)->heap,				\
		    MOD,					\
		    _lcfun_infor_##FNAME.name,			\
		    (vgc_obj*)cfun);				\
    }								\
  }while(0)

#define LDECLVAR(NAME) LVAR_INIT(ctx,mod,NAME)
  
#define LVAR_INIT(CTX,MOD,VNAME)				\
  do{								\
    ustring* str;						\
    vgc_obj* var;						\
    str = ustring_table_put((CTX)->symtb,			\
			    _lvar_infor_##VNAME.lname,		\
			    -1);				\
    if(!str){uabort("init var error!");}			\
    var = _lvar_infor_##VNAME.var_gen(CTX);			\
    if(var){							\
      vmod_lobj_put((CTX)->heap,MOD,str,var);			\
      if (_lvar_infor_##VNAME.scope == VPS_SCOPE_GLOBAL) {	\
	vmod_gobj_put((CTX)->heap,MOD,str,var);			\
      }								\
    }								\
  } while(0)

void lcfun_init(vcontext* ctx,vmod* mod);

#endif
