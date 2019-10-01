#include <stdio.h>
#include "ltoken.h"
#include "lobj.h"

ltoken_state ts;

int main(){
  vgc_heap* heap=vgc_heap_new(256,1024);
  int token;
  ts.buf = "( hello \"string\"  --123)";
  ts.pos = ts.buf;
  while(1){
    token = ltoken_next(&ts,heap);
    ltoken_log(&ts);
    if(token == ltk_eof || token == ltk_bad)
      break;
  }
  return 0;
}