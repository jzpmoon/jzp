#include "uhstb_tpl.h"
#include "ulist_tpl.h"
#include "ulr.h"
#include "vreader.h"

typedef struct _vir{
  int ir_no;
  ustring* ir_cid;
  ustring* ir_str;
  int ir_code;
  int ir_len;
  int ir_oct;
} vir;

typedef struct _vir_nterm{
  ustring* name;
  int no;
} vir_nterm;

ulist_decl_tpl(vir_nterm);

typedef ulist_vir_nterm vir_nterms;

uhstb_decl_tpl(vir);

typedef uhstb_vir virtb;

typedef struct _vir_attr_req{
  VAST_ATTR_REQ_HEADER;
  virtb* sirtb;
  virtb* dirtb;
  vir_nterms* nterms;
  ulrgram* gram;
} vir_attr_req;

virtb* virtb_new();

int virtb_put(virtb* irtb,vir ir);

int virtb_load(vreader* reader,virtb* irtb);

void vattr_init(vreader* reader);

ulrgram* vfile2gram(vreader* reader,char* file_path);

int vir_nterms_put(vir_nterms* nterms,ustring* name,int no);

vir_nterm* vir_nterm_get(vir_nterms* nterms,ustring* name);
