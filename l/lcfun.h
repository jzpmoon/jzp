#ifndef _LCFUN_H_
#define _LCFUN_H_

#include "vcontext.h"

struct _lcfun_infor{
  char* lname;
  ustring* name;
  int params_count;
  int has_retval;
  vcfun_ft entry;
};

typedef vgc_obj*(*lvar_gen_ft)();

struct _lvar_infor{
  char* lname;
  ustring* name;
  lvar_gen_ft var_gen;
};

#define RETVAL_YES 1

#define RETVAL_NO 0

#define LDEFUN(FNAME,LNAME,PCOUNT,RETVAL,BODY)		\
  int _lcfun_entry_##FNAME(vcontext* ctx){		\
    BODY;						\
  }							\
  static struct _lcfun_infor _lcfun_infor_##FNAME =	\
    {LNAME,NULL,PCOUNT,RETVAL,_lcfun_entry_##FNAME};

#define LDEFVAR(FNAME,LNAME,BODY)			\
  vgc_obj* _lvar_gen_##FNAME(){				\
    BODY;						\
  }							\
  static struct _lvar_infor _lvar_infor_##FNAME =	\
    {LNAME,NULL,_lvar_gen_##FNAME};  

#define LFUNONLOAD(fname,body)				\
  void _lcfun_file_init_##fname(vcontext* ctx){		\
    body						\
  }

#define LDECLFUN(NAME) LFUN_INIT(ctx,NAME)

#define LFUN_INIT(CTX,FNAME)					\
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
    vcontext_obj_put(CTX,					\
		     _lcfun_infor_##FNAME.name,			\
		     (vgc_obj*)cfun);				\
  }while(0)

#define LDECLVAR(NAME) LVAR_INIT(ctx,NAME)
  
#define LVAR_INIT(CTX,VNAME)				\
  do{							\
    ustring* str;					\
    vgc_obj* var;					\
    str = ustring_table_put((CTX)->symtb,		\
			    _lvar_infor_##VNAME.lname,	\
			    -1);			\
    if(!str){uabort("init var error!");}		\
    var = _lvar_infor_##VNAME.var_gen();		\
    if(var){						\
      vcontext_obj_put(CTX,str,var);			\
    }							\
  } while(0)

void lcfun_init(vcontext* ctx);

#endif
