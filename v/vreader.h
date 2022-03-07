#ifndef _VREADER_H_
#define _VREADER_H_

#include "ustring.h"
#include "ustream.h"
#include "ulist_tpl.h"
#include "uhstb_tpl.h"
#include "umempool.h"
#include "vgc_obj.h"

typedef struct _vast_obj vast_obj;
typedef struct _vast_attr_req vast_attr_req;
typedef struct _vast_attr_res vast_attr_res;
typedef struct _vast_attr vast_attr;
typedef struct _vast_kw vast_kw;
typedef struct _vreader vreader;
typedef struct _vtoken_state vtoken_state;

/*
 * return value: 0 nothing res, 1 have res.
 */
typedef int(*vast_attr_ft)(vast_attr_req* req,
			   vast_attr_res* res);

typedef void (*vattr_init_ft)(vreader*);


#define VASTHEADER \
  int t

struct _vast_obj{
  VASTHEADER;
};

#define VAST_ATTR_REQ_HEADER \
  vast_attr_ft req_from;     \
  struct _vreader* reader;   \
  vast_obj* ast_obj

struct _vast_attr_req{
  VAST_ATTR_REQ_HEADER;
};

#define vast_req_dbl(req) *(req)

enum var_type{
  var_apd,
};

/*
 * field member "res_type" from above enum.
 */
struct _vast_attr_res{
  vast_attr_ft res_from;
  void* res_obj;
  int res_type;
};

struct _vast_attr{
  char* sname;
  ustring* name;
  vast_attr_ft action;
};

int vast_attr_call(vast_attr* attr,vast_attr_req* req,vast_attr_res* res);

uhstb_decl_tpl(vast_attr);

struct _vast_kw{
  int kw_type;
  ucharp kw_str;
};

ulist_decl_tpl(vast_kw);

struct _vreader{
  umem_pool mp;
  ustring_table* symtb;
  ustring_table* strtb;
  uhstb_vast_attr* attrtb;
  vattr_init_ft ainit;
  vast_attr* dattr;
  ulist_vast_kw* kws;
  ufile_infor fi;
};

#define vreader_alloc_get(reader) \
  &(reader)->mp.allocator

struct _vtoken_state{
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
  vast_kw kw;
  vreader* reader;
  struct {
    int x;
    int y;
  } coord;
};

#define VEOF (-1)

vreader* vreader_new(ustring_table* symtb,
		     ustring_table* strtb,
		     vattr_init_ft ainit,
		     vast_attr* dattr);

vreader* vreader_easy_new(vattr_init_ft ainit);

int vreader_keyword_put(vreader* reader,vast_kw keyword);

vtoken_state* vreader_from(vreader* reader);

void vreader_clean(vreader* reader);

ustring* vreader_path_get(vreader* reader,ustring* name);

vtoken_state* vtoken_state_new(ustream* stream,
			       vreader* reader);

vtoken_state* vtoken_state_alloc(uallocator* allocator,
				 ustream* stream,
				 vreader* reader);

void vtoken_state_init(vtoken_state* ts);

void vtoken_state_close(vtoken_state* ts);

void vtoken_state_reset(vtoken_state* ts,ustring* file_path);

void vtoken_log(vtoken_state* ts);

#endif
