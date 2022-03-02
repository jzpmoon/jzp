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
  vreader* conf_reader;
  leval_loader loader;
};

typedef void(*lkw_init_ft)(vreader* reader);

UDECLFUN(UFNAME lstartup,
	 UARGS (vattr_init_ft attr_init,
		vattr_init_ft conf_attr_init,
		vcfun_init_ft cfun_init,
		lkw_init_ft kw_init,
		vps_prod_ft prod,
		vast_attr* symcall),
	 URET leval*);

UDECLFUN(UFNAME leval_load,
	 UARGS (leval* eval,char* file_path),
	 URET int);

UDECLFUN(UFNAME leval_conf_load,
	 UARGS (leval* eval,char* file_path),
	 URET int);

#endif
