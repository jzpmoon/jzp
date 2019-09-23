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
