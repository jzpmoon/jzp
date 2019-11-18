#include <stdio.h>

#include "uerror.h"
#include "vgc_obj.h"

int main() {
  int i = 0;
  int retval = 0;
  vslot slot;
  vgc_heap* heap = vgc_heap_new(512,1024*4,-1);
  while(retval == 0){
    retval = vgc_array_new(heap,10,vgc_heap_area_active);
    ulog("new array");
    if(i == 3)
      vgc_heap_stack_pop(heap,&slot);
    i++;
  }
  ustack_log_vslot(&heap->root_set);
  ulog("test end");
  return 0;
}
