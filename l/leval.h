#ifndef _LEVAL_H_
#define _LEVAL_H_

#include "ulib.h"
#include "vm.h"
#include "vparser.h"
#include "vcfun.h"

typedef struct _leval leval;
typedef struct _leval_loader leval_loader;
typedef struct _vast_conf_req vast_conf_req;

typedef void(*lmod_init_ft)(vcontext* ctx,vmod* mod);
typedef void(*lkw_init_ft)(vreader* reader);

struct _leval_loader{
  VMOD_LOADER_HEADER;
  leval* eval;
  vps_prod_ft prod;
};

struct _leval{
  ustring* self_path;
  vgc_heap* heap;
  vcontext* ctx;
  vps_cntr vps;
  vreader* src_reader;
  vreader* lib_reader;
  vreader* conf_reader;
  leval_loader loader;
};

struct _vast_conf_req{
  VAST_ATTR_REQ_HEADER;
  leval* eval;
};

UDECLFUN(UFNAME lstartup,
	 UARGS (char* self_path,
		vattr_init_ft attr_init,
		vattr_init_ft conf_attr_init,
		lkw_init_ft kw_init,
		vps_prod_ft prod,
		vast_attr* symcall),
	 URET leval*);

UDECLFUN(UFNAME leval_src_load,
	 UARGS (leval* eval,char* file_path),
	 URET int);

UDECLFUN(UFNAME leval_lib_load,
	 UARGS (leval* eval,char* file_path),
	 URET int);

UDECLFUN(UFNAME leval_conf_load,
	 UARGS (leval* eval,char* file_path),
	 URET int);

#endif
