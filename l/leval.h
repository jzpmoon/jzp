#ifndef _LEVAL_H_
#define _LEVAL_H_

#include "vvm.h"

int lstartup(vvm* vm,vslot* args);

struct _lcfun_infor{
  char* lname;
  int args_count;
  vcfun_t entry;
};

#define LDEFUN(FNAME,LNAME,ARGS_COUNT,BODY)		\
  int _lcfun_entry_##FNAME(vcontext* ctx){		\
    BODY;						\
  }							\
  static struct _lcfun_infor _lcfun_infor_##FNAME =	\
    {LNAME,ARGS_COUNT,_lcfun_entry_##FNAME};

#define LFUN_INIT(VM,FNAME)					\
  do{								\
    struct _lcfun_infor cfun_infor = _lcfun_infor_##FNAME;	\
    vslot* cfun;						\
    cfun = vgc_cfun_new((VM)->heap,				\
			vslot_ref_get((VM)->context->stack),	\
			cfun_infor.args_count,			\
			cfun_infor.entry,			\
			area_static);				\
    if(!cfun){							\
      uabort("LFUN_INIT:cfun new error!");			\
    }								\
    vvm_obj_put(VM,						\
		cfun_infor.lname,				\
		cfun);						\
  }while(0)

#endif
