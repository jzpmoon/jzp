#include "lobj.h"

int main(){
  vgc_heap* heap=vgc_heap_new(256,1024);
  lcons* cons = lcons_new(heap);
  return 0;
}
