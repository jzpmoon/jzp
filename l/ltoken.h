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
  int token;
  ustring* str;
  ustring* sym;
  double num;
  int bool;
  ustring_table* symtb;
  int symtb_size;
  ustring_table* strtb;
  int strtb_size;
  struct {
    int x;
    int y;
  } coord;
} ltoken_state;

#define LEOF (-1)

#define LASTHEADER \
  int t

typedef struct _last_obj{
  LASTHEADER;
} last_obj;

ltoken_state* ltoken_state_new(ustream* stream,
			       ustring_table* symtb,
			       int symtb_size,
			       ustring_table* strtb,
			       int strtb_size);

void ltoken_state_init(ltoken_state* ts,
		       ustream* stream);

last_obj* lparser_parse(ltoken_state* ts);

void ltoken_log(ltoken_state* ts);

void last_obj_log(last_obj* ast_obj);

enum {
  lastk_cons,
  lastk_symbol,
  lastk_number,
  lastk_string,
};

typedef struct _last_cons{
  LASTHEADER;
  last_obj* car;
  last_obj* cdr;
} last_cons;

typedef struct _last_symbol{
  LASTHEADER;
  ustring* name;
} last_symbol;

typedef struct _last_number{
  LASTHEADER;
  double dnum;
} last_number;

typedef struct _last_string{
  LASTHEADER;
  ustring* string;
} last_string;

last_cons* last_cons_new(last_obj* car,last_obj* cdr);

last_symbol* last_symbol_new(ustring* name);

last_number* last_number_new(double dnum);

last_string* last_string_new(ustring* string);

#endif
