#ifndef _LTOKEN_H_
#define _LTOKEN_H_

enum ltoken{
  ltk_indentify,
  ltk_left,
  ltk_right,
  ltk_string,
  ltk_number,
};

typedef struct _ltoken_state{
  char* buf;
  char* pos;
  int token;
  struct {
    int x;
    int y;
  } coord;
} ltoken_state;

int ltoken_next(ltoken* token);

vgc_obj* lparser_parse(ltoken* token);

#endif
