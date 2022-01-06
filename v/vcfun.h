#ifndef _VCFUN_H_
#define _VCFUN_H_

#include "vcontext.h"

struct _vcfun_infor{
  char* lname;
  ustring* name;
  int params_count;
  int has_retval;
  int scope;
  vcfun_ft entry;
};

typedef vgc_obj*(*var_gen_ft)();

struct _var_infor{
  char* lname;
  ustring* name;
  int scope;
  var_gen_ft var_gen;
};

typedef void (*vcfun_init_ft)(vcontext* ctx,vmod* mod);

#define VSCOPE_GLOBAL VPS_SCOPE_GLOBAL
#define VSCOPE_LOCAL VPS_SCOPE_LOCAL
#define VRETVAL_YES 1
#define VRETVAL_NO 0

#define VDEFUN(FNAME,LNAME,SCOPE,PCOUNT,RETVAL,BODY)	\
  int _vcfun_entry_##FNAME(vcontext* ctx){		\
    BODY;						\
  }							\
  static struct _vcfun_infor _vcfun_infor_##FNAME =	\
    {LNAME,NULL,PCOUNT,RETVAL,SCOPE,_vcfun_entry_##FNAME};

#define VDEFVAR(FNAME,LNAME,SCOPE,BODY)			\
  vgc_obj* _var_gen_##FNAME(vcontext* ctx){		\
    BODY;						\
  }							\
  static struct _var_infor _var_infor_##FNAME =	\
    {LNAME,NULL,SCOPE,_var_gen_##FNAME};  

#define VFUNONLOAD(fname,body)					\
  void _vcfun_file_init_##fname(vcontext* ctx,vmod* mod){	\
    body							\
  }

#define VDECLFUN(NAME) VFUN_INIT(ctx,mod,NAME)

#define VFUN_INIT(CTX,MOD,FNAME)				\
  do{								\
    vgc_cfun* cfun;						\
    ustring* str;						\
    str = ustring_table_put((CTX)->symtb,			\
			    _vcfun_infor_##FNAME.lname,		\
			    -1);				\
    if(!str){uabort("init cfun error!");}			\
    _vcfun_infor_##FNAME.name = str;				\
    cfun = vgc_cfun_new((CTX)->heap,				\
			_vcfun_infor_##FNAME.entry,		\
			_vcfun_infor_##FNAME.params_count,	\
      			_vcfun_infor_##FNAME.has_retval,	\
			vgc_heap_area_static);			\
    if(!cfun){							\
      uabort("LFUN_INIT:cfun new error!");			\
    }								\
    vmod_lobj_put((CTX)->heap,					\
		  MOD,						\
		  _vcfun_infor_##FNAME.name,			\
		  (vgc_obj*)cfun);				\
    if (_vcfun_infor_##FNAME.scope == VPS_SCOPE_GLOBAL) {	\
      vmod_gobj_put((CTX)->heap,				\
		    MOD,					\
		    _vcfun_infor_##FNAME.name,			\
		    (vgc_obj*)cfun);				\
    }								\
  }while(0)

#define VDECLVAR(NAME) VAR_INIT(ctx,mod,NAME)
  
#define VAR_INIT(CTX,MOD,VNAME)				\
  do{								\
    ustring* str;						\
    vgc_obj* var;						\
    str = ustring_table_put((CTX)->symtb,			\
			    _var_infor_##VNAME.lname,		\
			    -1);				\
    if(!str){uabort("init var error!");}			\
    var = _var_infor_##VNAME.var_gen(CTX);			\
    if(var){							\
      vmod_lobj_put((CTX)->heap,MOD,str,var);			\
      if (_var_infor_##VNAME.scope == VPS_SCOPE_GLOBAL) {	\
	vmod_gobj_put((CTX)->heap,MOD,str,var);			\
      }								\
    }								\
  } while(0)

#endif
