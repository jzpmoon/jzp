#ifndef _LTOKEN_H_
#define _LTOKEN_H_

#include "ustring.h"
#include "ustream.h"
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
  ustream* stream;
  ubuffer* buff;
  int      token;
  vslot*   str;
  ustring* sym;
  double   num;
  int      bool;
  struct {
    int x;
    int y;
  }        coord;
} ltoken_state;

#define LEOF (-1)

ltoken_state* ltoken_state_new(ustream* stream);

void ltoken_state_init(ltoken_state* ts,
		       ustream* stream);

vslot* lparser_parse(ltoken_state* ts,
		     vgc_heap* heap,
		     vgc_stack* stack);

void ltoken_log(ltoken_state* ts);

void lparser_exp_log(vslot* slotp_s_exp);

#endif
