#include <stddef.h>

#include "ualloc.h"
#include "uerror.h"
#include "vpass.h"
#include "vcontext.h"
#include "vm.h"

#define INST(code,operand) \
  insts=ulist_append(insts,&((vinst){code,operand}))

int main(){
  vgc_heap* heap = NULL;
  vcontext* ctx = NULL;
  ulist* insts = NULL;
  vgc_array* consts;
  vgc_subr* subr;
  vslot subr_slot;
  vslot slot;
  heap = vgc_heap_new(512,1024*4,-1);
  if(!heap){
    uabort("new heap error!");
  }
  ctx = vcontext_new(heap);
  if(!ctx){
    uabort("new vcontext error!");
  }
  consts = vgc_obj_ref_get(ctx,consts,vgc_array);
  vslot_num_set(consts->objs[0],1);
  INST(Bload,0);
  INST(Bload,1);
  INST(Beq,-1);
  INST(Bjmpi,7);
  INST(Bload,0);
  INST(Bpush,0);
  INST(Bload,1);
  INST(Badd,-1);
  INST(Bpush,1);
  INST(Bcall,1);
  INST(Bretvoid,-1);
  vslot_num_set(slot,4);
  vgc_heap_stack_push(heap,slot);
  vslot_num_set(slot,1);
  vgc_heap_stack_push(heap,slot);
  if(vinst_to_str(ctx,insts)){
    uabort("new vinst_to_str error!");
  }
  vgc_heap_obj_push(heap,consts);
  subr = vgc_subr_new(heap,2,0,vgc_heap_area_active);
  if(!subr){
    uabort("new vgc_subr error!");
  }
  vgc_heap_obj_push(heap,subr);
  subr_slot = vgc_heap_stack_get(heap,-1);
  consts->objs[1] = subr_slot;
  ulog("execute");
  vcontext_execute(ctx);
  return 0;
}
