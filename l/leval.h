#ifndef _LEVAL_H_
#define _LEVAL_H_

#include "vm.h"
#include "vparser.h"
#include "vcfun.h"

typedef struct _leval leval;

typedef struct _leval_loader{
  VMOD_LOADER_HEADER;
  leval* eval;
  vps_prod_ft prod;
} leval_loader;

struct _leval{
  vgc_heap* heap;
  vcontext* ctx;
  vmod* mod;
  vps_cntr vps;
  vreader* reader;
  leval_loader loader;
};

enum leval_kw{
  #define DF(no,str) no,
  #include "lkw.h"
  #undef DF
};

UDECLFUN(UFNAME lstartup,
	 UARGS (vattr_init_ft attr_init,
		vcfun_init_ft cfun_init,
		vps_prod_ft prod,
		vast_attr* symcall),
	 URET leval*);

UDECLFUN(UFNAME leval_load,
	 UARGS (leval* eval,char* file_path),
	 URET int);

#endif
