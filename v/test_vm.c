#include <stddef.h>

#include "vvm.h"
#include "ualloc.h"
#include "uerror.h"

#define heap_size 1024*10

#define INST(code,operand) \
  insts=ulist_append(insts,&((vinst){code,operand}))

int main(){
  vslot slot_num;
  vslot slot_num1;
  vslot slot_num2;
  vslot slot_stk;
  vslot* slotp_subr;
  vslot slot_stack;
  vgc_stack* stk;
  ulist* insts=NULL;
  vgc_stack* stack;
  vgc_subr* subr;
  vgc_heap* heap;
  vcontext* ctx;
  vvm* vm;

  vm=vvm_new(heap_size,heap_size,100,100);
  if(!vm){
    uabort("vm new error!");
  }
  heap=vm->heap;
  ctx = vm->context;
  
  INST(Bpush,1); /* 2 */
  INST(Bref,0); /* 3 */
  INST(Bpush,2);  /* 2 */
  INST(Beq,-1);/* 1 */
  INST(Bjmpi,9); /* 3 */
  INST(Bpush,1); /* 2 */
  INST(Bref,0); /* 3 */
  INST(Bpush,3); /* 2 */
  INST(Badd,-1); /* 1 */
  INST(Bpush,1);/* 2 */
  INST(Bset,0); /* 3 */
  INST(Bpush,0);  /* 2 */
  INST(Bcall,-1); /* 1 */
  INST(Bretvoid,-1); /* 1 */

  stack = vm->consts;
  vslot_ref_set(slot_stack,stack);

  slotp_subr=vgc_subr_new(heap,
			  ctx->cache,
			  0,
			  1,
			  vinst_to_str(ctx,
				       insts),
			  &slot_stack);
  subr = vslot_ref_get(*slotp_subr);
  vslot_num_set(slot_num,0);
  stk = vgc_stack_new(heap,ctx->cache,1,area_active);
  vslot_ref_set(slot_stk,stk);
  vgc_stack_push(stk,slot_num);
  vslot_num_set(slot_num1,4);
  vslot_num_set(slot_num2,1);
  vgc_stack_push(stack,*slotp_subr);
  vgc_stack_push(stack,slot_stk);
  vgc_stack_push(stack,slot_num1);
  vgc_stack_push(stack,slot_num2);
  /*vgc_collect(heap);*/
  vcontext_cache_clean(ctx);
  vcontext_execute(vm->context,slotp_subr);
  return 0;
}
