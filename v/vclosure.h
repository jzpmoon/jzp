#ifndef _VCLOSURE_H_
#define _VCLOSURE_H_

#include "ulist_tpl.h"
#include "vpass.h"

typedef struct _vclosure vclosure,*vclosurep;

ulist_decl_tpl(vclosurep);

struct _vclosure{
  ulist_vps_datap* fields;
  vcfg_graph* init;
  vclosure* parent;
  ulist_vclosurep* childs;
};

typedef struct _vps_closure_req{
  VAST_ATTR_REQ_HEADER;
  vps_cntr* vps;
  vclosure* closure;
} vps_closure_req;

UDECLFUN(UFNAME vclosure_new,
	 UARGS (vps_cntr* vps),
	 URET vclosure*);
#endif
