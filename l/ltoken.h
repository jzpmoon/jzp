#ifndef _LTOKEN_H_
#define _LTOKEN_H_

#include "ustring.h"
#include "ustream.h"
#include "uhstb_tpl.h"
#include "umempool.h"
#include "vgc_obj.h"

enum ltoken{
  ltk_bad,
  ltk_identify,
  ltk_left,
  ltk_right,
  ltk_string,
  ltk_number,
  ltk_quote,
  ltk_eof,
};

typedef struct _ltoken_state{
  umem_pool* pool;
  ustream* stream;
  ubuffer* buff;
  int token;
  ustring* str;
  ustring* id;
  double num;
  int bool;
  ustring_table* symtb;
  ustring_table* strtb;
  struct _uhstb_last_attr* attrtb;
  struct {
    int x;
    int y;
  } coord;
} ltoken_state;

#define LEOF (-1)

ltoken_state* ltoken_state_new(ustream* stream,
			       ustring_table* symtb,
			       ustring_table* strtb);

void ltoken_state_init(ltoken_state* ts,
		       ustream* stream);

void ltoken_state_reset(ltoken_state* ts,FILE* file);

void ltoken_log(ltoken_state* ts);

#endif
