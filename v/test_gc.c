#include "uerror.h"
#include "vgc_obj.h"

int main() {

  vgc_heap* heap   = vgc_heap_new(512,1024*4);
  vgc_stack* stack = vgc_stack_new(heap,10);
  int i            = 0;
  int j            = 0;
  while(i<10) {
    vslot slot = {vslot_type_num,{i}};
    vgc_stack_push(stack,slot);
    i++;
  }
  while(j<10) {
    vslot slot = vgc_stack_pop(stack);
    ulog1("num:%f",slot.u.num);
    j++;
  }
  return 0;
  
}
