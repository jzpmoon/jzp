#include "ltoken.h"

#define ltoken_next_char(ts) \
  *(ts)->pos++;(ts)->coord.x++
#define ltoken_new_line(ts) \
  (ts)->coord.x=0;(ts)->coord.y++
#define ltoken_back_char(ts) \
  (ts)->pos--;(ts)->coord.x--

void ltoken_skip_blank(ltoken_state* ts){
  int c;
  while(1){
    c = ltoken_next_char(ts);
    if(c == ' ' || c == '\t' || c== '\r'){
      continue;
    }if(c== '\n'){
      ltoken_new_line(ts);
      continue;
    }else{
      ltoken_back_char(ts);
      break;
    }
  }
}

int ltoken_lex_string(ltoken_state* ts,vgc_heap* heap){
  vgc_str* str;
  char*    begin;
  int      len;
  int      c;
  begin = ts->pos;
  while(1){
    c = ltoken_next_char(ts);
    if(c == '"'){
      break;
    }
  }
  len = ts->pos - begin - 1;
  str = vgc_str_newc(heap,begin,len);
  if(!str){
    uabort("lparse: new vgc_str error!");
  }
  ts->str = str;
  return ts->token = ltk_string;
}

int ltoken_lex_number(ltoken_state* ts){
  ustring* str;
  char* begin;
  int   len;
  int   dot;
  int   c;
  begin = ts->pos - 1;
  while(1){
    c = ltoken_next_char(ts);
    if(c == '.'){
      dot++;
    }
    if(dot > 1){
      return ts->token = ltk_bad;
    }
    if(c < '0' && c > '9'){
      ltoken_back_char(ts);
      break;
    }
  }
  len = ts->pos - begin;
  str = vstrtb_put(begin,len);
  if(!str){
    uabort("lparse: vstrtb_put error!");
  }
  return ts->token = ltk_number;
}
