#include <stddef.h>

#include "vm.h"
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
  vslot slot_subr;
  vgc_stack* stk;
  ulist* insts=NULL;
  vgc_stack* stack;
  vgc_subr* subr;
  vgc_heap* heap;
  vm* vm;

  vm=vm_new(heap_size,heap_size,100,100);
  if(!vm){
    uabort("vm new error!");
  }
  
  heap=vm->heap;
  
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
  subr=vgc_subr_new( heap,
		    0,
		    1,
		    vinst_to_str( heap,
				 insts),
		    stack);
  vslot_num_set(slot_num,0);
  stk = vgc_stack_new( heap,1);
  vslot_ref_set(slot_stk,stk);
  vgc_stack_push(stk,slot_num);
  vslot_ref_set(slot_subr,subr);
  vslot_num_set(slot_num1,4);
  vslot_num_set(slot_num2,1);
  vgc_stack_push(stack,slot_subr);
  vgc_stack_push(stack,slot_stk);
  vgc_stack_push(stack,slot_num1);
  vgc_stack_push(stack,slot_num2);
  /*vgc_collect(heap);*/

  vcontext_execute(vm->context,(vgc_obj*)subr);
  return 0;
}
