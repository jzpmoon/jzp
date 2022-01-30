#ifndef _VCLOSURE_H_
#define _VCLOSURE_H_

#include "ulist_tpl.h"
#include "vpass.h"

typedef struct _vclosure vclosure,*vclosurep;

ulist_decl_tpl(vclosurep);

#define VCLOSURE_TYPE_FILE 1
#define VCLOSURE_TYPE_FUNC 2
#define VCLOSURE_TYPE_MAIN 3
#define VCLOSURE_TYPE_NONE 4
#define VCLOSURE_TYPE_DECL 5

struct _vclosure{
  ustring* closure_name;
  ulist_vps_datap* fields;
  vcfg_graph* init;
  vclosure* parent;
  ulist_vclosurep* childs;
  int closure_type;
};

typedef struct _vps_closure_req{
  VAST_ATTR_REQ_HEADER;
  vps_cntr* vps;
  vclosure* closure;
} vps_closure_req;

#define vclosure_isfile(closure)			\
  ((closure)->closure_type == VCLOSURE_TYPE_FILE ||	\
   (closure)->closure_type == VCLOSURE_TYPE_MAIN ||	\
   (closure)->closure_type == VCLOSURE_TYPE_DECL)

#define vclosure_ismain(closure)			\
  ((closure)->closure_type == VCLOSURE_TYPE_MAIN)

UDECLFUN(UFNAME vclosure_new,
	 UARGS (vps_cntr* vps),
	 URET vclosure*);

UDECLFUN(UFNAME vfile2closure,
	 UARGS (vclosure* closure,vreader* reader,char* file_path,
		vps_cntr* vps,int closure_type),
	 URET vclosure*);

UDECLFUN(UFNAME vclosure2vps,
	 UARGS (vreader* reader,char* file_path,vps_cntr* vps),
	 URET vps_mod*);

UDECLFUN(UFNAME vclosure_field_add,
	 UARGS (vclosure* closure,vps_data* field),
	 URET int);

UDECLFUN(UFNAME vclosure_child_add,
	 UARGS (vclosure* closure,vclosure* child),
	 URET int);

UDECLFUN(UFNAME vclosure_field_get,
	 UARGS (vclosure* closure,ustring* name),
	 URET vps_data*);

UDECLFUN(UFNAME vclosure_curr_field_get,
	 UARGS (vclosure* closure,ustring* name),
	 URET vps_data*);

#endif
