#ifndef _VREADER_H_
#define _VREADER_H_

#include "ustring.h"
#include "ustream.h"
#include "uhstb_tpl.h"
#include "umempool.h"
#include "vgc_obj.h"
#include "vpass.h"

#define VASTHEADER \
  int t

typedef struct _vast_obj{
  VASTHEADER;
} vast_obj;

enum { var_vps_apd };

typedef struct _vast_attr_req{
  vps_cntr* vps;
  vps_mod* top;
  vps_cfg* parent;
  struct _vreader* reader;
  vast_obj* ast_obj;
} vast_attr_req;

/*
 * field member "res_type" from above enum.
 */
typedef struct _vast_attr_res{
  vps_t* res_vps;
  int res_type;
} vast_attr_res;

/*
 * return value: 0 nothing res, 1 have res.
 */
typedef int(*vast_attr_ft)(vast_attr_req* req,
			   vast_attr_res* res);

typedef struct _vast_attr{
  char* sname;
  ustring* name;
  vast_attr_ft action;
} vast_attr;

uhstb_decl_tpl(vast_attr);

typedef struct _vtoken_state{
  umem_pool mp;
  uallocator* allocator;
  ustream* stream;
  ubuffer* buff;
  int c;
  int token;
  ustring* str;
  ustring* id;
  double dnum;
  int inte;
  int chara;
  int bool;
  ustring_table* symtb;
  ustring_table* strtb;
  uhstb_vast_attr* attrtb;
  struct {
    int x;
    int y;
  } coord;
} vtoken_state;

#define VEOF (-1)

typedef void (*vattr_init_ft)(vtoken_state*);

typedef struct _vreader{
  umem_pool mp;
  ustring_table* symtb;
  ustring_table* strtb;
  uhstb_vast_attr* attrtb;
  vattr_init_ft ainit;
} vreader;

vreader* vreader_new(ustring_table* symtb,
		     ustring_table* strtb,
		     vattr_init_ft ainit);

struct _vtoken_state* vreader_from(vreader* reader);

void vreader_clean(vreader* reader);

vtoken_state* vtoken_state_new(ustream* stream,
			       ustring_table* symtb,
			       ustring_table* strtb,
			       uhstb_vast_attr* attrtb,
			       vattr_init_ft ainit);

vtoken_state* vtoken_state_alloc(uallocator* allocator,
				 ustream* stream,
				 ustring_table* symtb,
				 ustring_table* strtb,
				 uhstb_vast_attr* attrtb,
				 vattr_init_ft ainit);

void vtoken_state_init(vtoken_state* ts);

void vtoken_state_close(vtoken_state* ts);

void vtoken_state_reset(vtoken_state* ts,FILE* file);

void vtoken_log(vtoken_state* ts);

#endif
