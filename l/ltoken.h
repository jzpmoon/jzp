#ifndef _LTOKEN_H_
#define _LTOKEN_H_

#include "ustring.h"
#include "vgc_obj.h"
#include "lobj.h"

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
  char*    buf;
  char*    pos;
  int      token;
  vgc_str* str;
  ustring* sym;
  double   num;
  int      bool;
  struct {
    int x;
    int y;
  }        coord;
} ltoken_state;

#define LEOF ('\0')

int ltoken_next(ltoken_state* ts,vgc_heap* heap);

vslot lparser_parse(ltoken_state* ts,vgc_heap* heap);

void ltoken_log(ltoken_state* ts);

#endif
