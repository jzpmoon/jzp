#ifndef _LREADER_H_
#define _LREADER_H_

#include "ustring.h"
#include "ustream.h"
#include "uhstb_tpl.h"
#include "umempool.h"
#include "vgc_obj.h"
#include "vpass.h"

enum {
  lastk_cons,
  lastk_symbol,
  lastk_integer,
  lastk_number,
  lastk_string,
};

#define LASTHEADER \
  int t

typedef struct _last_obj{
  LASTHEADER;
} last_obj;

enum{
  lar_vps_apd,
  lar_blk_new,
};

typedef struct _last_attr_req{
  vps_cntr* vps;
  vps_mod* top;
  vps_dfg* parent;
  struct _lreader* reader;
  last_obj* ast_obj;
} last_attr_req;

/*
 * field member "res_type" from above enum.
 */
typedef struct _last_attr_res{
  vps_t* res_vps;
  int res_type;
} last_attr_res;

/*
 * return value: 0 nothing res, 1 have res.
 */
typedef int(*last_attr_ft)(last_attr_req* req,
			   last_attr_res* res);

typedef struct _last_attr{
  char* sname;
  ustring* name;
  last_attr_ft action;
} last_attr;

uhstb_decl_tpl(last_attr);

enum ltoken{
  ltk_bad,
  ltk_identify,
  ltk_left,
  ltk_right,
  ltk_string,
  ltk_integer,
  ltk_number,
  ltk_quote,
  ltk_eof,
};

typedef struct _ltoken_state{
  umem_pool mp;
  uallocator* allocator;
  ustream* stream;
  ubuffer* buff;
  int token;
  ustring* str;
  ustring* id;
  int inte;
  double dnum;
  int bool;
  ustring_table* symtb;
  ustring_table* strtb;
  uhstb_last_attr* attrtb;
  struct {
    int x;
    int y;
  } coord;
} ltoken_state;

#define LEOF (-1)

typedef struct _lreader{
  umem_pool mp;
  ustring_table* symtb;
  ustring_table* strtb;
  uhstb_last_attr* attrtb;
} lreader;

lreader* lreader_new(ustring_table* symtb,
		     ustring_table* strtb);

struct _ltoken_state* lreader_from(lreader* reader);

void lreader_clean(lreader* reader);

ltoken_state* ltoken_state_new(ustream* stream,
			       ustring_table* symtb,
			       ustring_table* strtb,
			       uhstb_last_attr* attrtb);

ltoken_state* ltoken_state_alloc(uallocator* allocator,
				 ustream* stream,
				 ustring_table* symtb,
				 ustring_table* strtb,
				 uhstb_last_attr* attrtb);

void ltoken_state_init(ltoken_state* ts);

void ltoken_state_close(ltoken_state* ts);

void ltoken_state_reset(ltoken_state* ts,FILE* file);

void ltoken_log(ltoken_state* ts);

#endif
