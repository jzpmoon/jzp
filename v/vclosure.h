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

extern vast_attr vclosure_attr_symcall;

UDECLFUN(UFNAME vclosure_new,
	 UARGS (vps_cntr* vps),
	 URET vclosure*);

UDECLFUN(UFNAME vclosure2vps,
	 UARGS (vreader* reader,char* file_path,vps_cntr* vps),
	 URET vps_mod*);

UDECLFUN(UFNAME vclosure_child_add,
	 UARGS (vclosure* closure,vclosure* child),
	 URET int);

UDECLFUN(UFNAME vclosure_field_get,
	 UARGS (vclosure* closure,ustring* name),
	 URET vps_data*);

#endif
