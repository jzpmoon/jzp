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

#define RETVAL_YES 1

#define RETVAL_NO 0

#define LDEFUN(FNAME,LNAME,PCOUNT,RETVAL,BODY)		\
  int _lcfun_entry_##FNAME(vcontext* ctx){		\
    BODY;						\
  }							\
  static struct _lcfun_infor _lcfun_infor_##FNAME =	\
    {LNAME,NULL,PCOUNT,RETVAL,_lcfun_entry_##FNAME};

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

#define LVAR_INIT(CTX,NAME,VAR)					\
  do{								\
    ustring* str;						\
    str = ustring_table_put((CTX)->symtb,			\
			    NAME);				\
    if(!str){uabort("init var error!");}			\
    vcontext_obj_put(CTX,NAME,slotp);				\
  } while(0)

void lcfun_init(vcontext* ctx);

#endif
