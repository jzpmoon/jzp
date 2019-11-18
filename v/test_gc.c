#include "uerror.h"
#include "vgc_obj.h"

int main() {

  vgc_heap* heap = vgc_heap_new(512,1024*4,-1);
  vgc_array_new(heap,10,vgc_heap_area_static);
  return 0;
}
