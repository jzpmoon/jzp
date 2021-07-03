#include <stdio.h>

#include "uerror.h"
#include "vgc_obj.h"

void vgc_heap_stack_log(vgc_heap* heap){
  ustack_vslot* root_set = &heap->root_set;
  ublock_vslot* block = root_set->curr_block;
  int i = 0;
  ulog1("stack top:%d",root_set->block_pos);
  while(i < root_set->block_pos){
    vslot slot = block->ptr[i];
    ulog1("stack->[%d]",i);
    vslot_log(slot);
    if(vslot_is_ref(slot)){
      vgc_obj* obj = vslot_ref_get(slot,vgc_obj);
      vgc_obj_log(obj);
    }
    i++;
  }
}

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
  ustack_vslot_log(&heap->root_set);
  ulog("test end");
  return 0;
}
