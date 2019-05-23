#include <stddef.h>

#include "vm.h"
#include "ualloc.h"
#include "uerror.h"

#define heap_size 1024*10
vgc_heap heap={
	      heap_size,
	      NULL,
	      NULL,
	      NULL,
	      {NULL,NULL,0,0,4}
};

#define INST(code,operand) insts=ulist_append(insts,&((vinst){code,operand}))

int main(){
  void* mem;
  ulist* insts=NULL;
  vgc_stack* stack;
  vgc_subr* subr;
  vcontext* ctx;
  unew(mem,heap_size,return -1;);
  heap.mem=heap.end=mem;
  
  INST(Bpush,1); /* 2 */
  INST(Bref,0); /* 3 */
  INST(Bpush,2);  /* 4 */
  INST(Beq,-1);/* 5 */
  INST(Bjmpi,27); /* 6 */
  INST(Bpush,1); /* 7 */
  INST(Bref,0); /* 7 */
  INST(Bpush,3); /* 7 */
  INST(Badd,-1); /* 8 */
  INST(Bpush,1);/* 9 */
  INST(Bset,0); /* 7 */
  INST(Bpush,0);  /* 10 */
  INST(Bcall,-1); /* 7 */
  INST(Bretvoid,-1); /* 12 */

  stack=vgc_stack_new(&heap,4);
  subr=vgc_subr_new(&heap,
		    0,
		    1,
		    vinst_to_str(&heap,
				 insts),
		    stack);
  vgc_num* num=vgc_num_new(&heap,0);
  vgc_stack* stk=vgc_stack_new(&heap,1);
  stk->objs[0]=(vgc_obj*)num;
  vgc_stack_push(stack,(vgc_obj*)subr);
  vgc_stack_push(stack,(vgc_obj*)stk);
  vgc_stack_push(stack,(vgc_obj*)vgc_num_new(&heap,4));
  vgc_stack_push(stack,(vgc_obj*)vgc_num_new(&heap,1));
  ctx=vcontext_new(&heap);
  vcontext_execute(ctx,subr);
  return 0;
}
