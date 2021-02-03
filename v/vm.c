#include <stddef.h>
#include "ualloc.h"
#include "uerror.h"
#include "vm.h"

void bc_top(vcontext* ctx,usize_t index){
  vgc_heap_stack_top_set(ctx->heap,index);
}

void bc_push(vcontext* ctx,
	     vslot slot){
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_push(heap,slot);
}

void bc_pop(vcontext* ctx){
  vslot slot;
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_pop(heap,&slot);
}

vslot bc_locals(vcontext* ctx,
		usize_t index){
  vgc_call* calling = vgc_obj_ref_get(ctx,calling,vgc_call);
  vgc_subr* subr = vgc_obj_ref_get(calling,subr,vgc_subr);
  usize_t count = subr->params_count + subr->locals_count;
  usize_t base = calling->base;
  vslot slot;
  usize_t real_index = base - count + index;
  if(index < 0 || index >= count)
    uabort("vm:local varable error!");
  slot = vgc_heap_stack_get(ctx->heap,real_index);
  return slot;
}

void bc_store(vcontext* ctx,
	      usize_t index){
  vslot slot;
  vgc_heap* heap = ctx->heap;
  vgc_call* calling = vgc_obj_ref_get(ctx,calling,vgc_call);
  vgc_subr* subr = vgc_obj_ref_get(calling,subr,vgc_subr);
  usize_t count = subr->params_count + subr->locals_count;
  usize_t base = calling->base;
  usize_t real_index = base - count + index;
  if(index < 0 || index >= count)
    uabort("vm:local varable error!");
  vgc_heap_stack_pop(heap,&slot);
  vgc_heap_stack_set(heap,real_index,slot);
}

void bc_jmp(vcontext* ctx,
	    usize_t offset){
  vgc_call* calling = vgc_obj_ref_get(ctx,calling,vgc_call);
  vgc_subr* subr = vgc_obj_ref_get(calling,subr,vgc_subr);
  vgc_string* bytecode = vgc_obj_ref_get(subr,bytecode,vgc_string);
  if(!vgc_str_bound_check(bytecode,offset))
    uabort("vm:jmp error!");
  calling->pc = bytecode->u.b + offset;
}

void bc_jmpi(vcontext* ctx,
	     usize_t offset){
  vslot slot;
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_pop(heap,&slot);
  if(vslot_is_true(slot)){
    bc_jmp(ctx,offset);
  }
}

void bc_add(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  vslot num;
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_pop(heap,&slot1);
  vgc_heap_stack_pop(heap,&slot2);
  if(!vslot_is_num(slot1) ||
     !vslot_is_num(slot2)){
    uabort("vm:add not a number!");
  }
  num = vslot_num_add(slot1,slot2);
  vgc_heap_stack_push(heap,num);
}

void bc_sub(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  double num1;
  double num2;
  vslot num;
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_pop(heap,&slot1);
  vgc_heap_stack_pop(heap,&slot2);
  if(!vslot_is_num(slot1) ||
     !vslot_is_num(slot2)){
    uabort("vm:sub not a number!");
  }
  num1 = vslot_num_get(slot1);
  num2 = vslot_num_get(slot2);
  vslot_num_set(num,(num1-num2));
  vgc_heap_stack_push(heap,num);
}

void bc_mul(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  double num1;
  double num2;
  vslot num;
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_pop(heap,&slot1);
  vgc_heap_stack_pop(heap,&slot2);
  if(!vslot_is_num(slot1) ||
     !vslot_is_num(slot2)){
    uabort("vm:mul not a number!");
  }
  num1 = vslot_num_get(slot1);
  num2 = vslot_num_get(slot2);
  vslot_num_set(num,(num1*num2));
  vgc_heap_stack_push(heap,num);
}

void bc_div(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  double num1;
  double num2;
  vslot num;
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_pop(heap,&slot1);
  vgc_heap_stack_pop(heap,&slot2);
  if(!vslot_is_num(slot1) ||
     !vslot_is_num(slot2)){
    uabort("vm:div not a number!");
  }
  num1 = vslot_num_get(slot1);
  num2 = vslot_num_get(slot2);
  if(num2 == 0){
    uabort("denominator can't be zero");
  }
  vslot_num_set(num,(num1/num2));
  vgc_heap_stack_push(heap,num);
}

void bc_eq(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  vslot bool;

  vgc_heap_stack_pop(ctx->heap,&slot1);
  vgc_heap_stack_pop(ctx->heap,&slot2);

  if(vslot_is_num(slot1) && vslot_is_num(slot2)){
    bool = vslot_num_eq(slot1,slot2);
  }else if(vslot_is_ref(slot1) && vslot_is_ref(slot2)){
    bool = vslot_ref_eq(slot1,slot2);
  }else if(vslot_is_true(slot1) && vslot_is_true(slot2)){
    vslot_bool_set(bool,vbool_true);
  }else{
    vslot_bool_set(bool,vbool_false);
  }
  vgc_heap_stack_push(ctx->heap,bool);
}

void bc_gt(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  vslot bool;

  vgc_heap_stack_pop(ctx->heap,&slot1);
  vgc_heap_stack_pop(ctx->heap,&slot2);

  if(vslot_is_num(slot1) && vslot_is_num(slot2)){
    double num1 = vslot_num_get(slot1);
    double num2 = vslot_num_get(slot2);
    if(num1 > num2){
      vslot_bool_set(bool,vbool_true);
    }else{
      vslot_bool_set(bool,vbool_false);
    }
  }else{
    vslot_bool_set(bool,vbool_false);
  }
  vgc_heap_stack_push(ctx->heap,bool);
}

void bc_lt(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  vslot bool;

  vgc_heap_stack_pop(ctx->heap,&slot1);
  vgc_heap_stack_pop(ctx->heap,&slot2);

  if(vslot_is_num(slot1) && vslot_is_num(slot2)){
    double num1 = vslot_num_get(slot1);
    double num2 = vslot_num_get(slot2);
    if(num1 < num2){
      vslot_bool_set(bool,vbool_true);
    }else{
      vslot_bool_set(bool,vbool_false);
    }
  }else{
    vslot_bool_set(bool,vbool_false);
  }
  vgc_heap_stack_push(ctx->heap,bool);  
}

void bc_and(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  vslot bool;

  vgc_heap_stack_pop(ctx->heap,&slot1);
  vgc_heap_stack_pop(ctx->heap,&slot2);

  if(vslot_is_num(slot1) && vslot_is_num(slot2)){
    double num1 = vslot_num_get(slot1);
    double num2 = vslot_num_get(slot2);
    if(num1 && num2){
      vslot_bool_set(bool,vbool_true);
    }else{
      vslot_bool_set(bool,vbool_false);
    }
  }else if(vslot_is_bool(slot1) && vslot_is_bool(slot2)){
    int bool1 = vslot_bool_get(slot1);
    int bool2 = vslot_bool_get(slot2);
    if(bool1 && bool2){
      vslot_bool_set(bool,vbool_true);
    }else{
      vslot_bool_set(bool,vbool_false);
    }
  }else{
    vslot_bool_set(bool,vbool_false);
  }
  vgc_heap_stack_push(ctx->heap,bool);    
}

void bc_or(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  vslot bool;

  vgc_heap_stack_pop(ctx->heap,&slot1);
  vgc_heap_stack_pop(ctx->heap,&slot2);

  if(vslot_is_num(slot1) && vslot_is_num(slot2)){
    double num1 = vslot_num_get(slot1);
    double num2 = vslot_num_get(slot2);
    if(num1 || num2){
      vslot_bool_set(bool,vbool_true);
    }else{
      vslot_bool_set(bool,vbool_false);
    }
  }else if(vslot_is_bool(slot1) && vslot_is_bool(slot2)){
    int bool1 = vslot_bool_get(slot1);
    int bool2 = vslot_bool_get(slot2);
    if(bool1 || bool2){
      vslot_bool_set(bool,vbool_true);
    }else{
      vslot_bool_set(bool,vbool_false);
    }
  }else{
    vslot_bool_set(bool,vbool_false);
  }
  vgc_heap_stack_push(ctx->heap,bool);  
}

void bc_not(vcontext* ctx){
  vslot slot;
  vslot bool;
  
  vgc_heap_stack_pop(ctx->heap,&slot);

  if(vslot_is_num(slot)){
    double num = vslot_num_get(slot);
    if(!num){
      vslot_bool_set(bool,vbool_true);
    }else{
      vslot_bool_set(bool,vbool_false);
    }
  }else if(vslot_is_bool(slot)){
    int b = vslot_bool_get(slot);
    if(!b){
      vslot_bool_set(bool,vbool_true);
    }else{
      vslot_bool_set(bool,vbool_false);
    }
  }else{
    vslot_bool_set(bool,vbool_false);
  }
  vgc_heap_stack_push(ctx->heap,bool);  
}

vslot bc_constant(vcontext* ctx,
		  usize_t index){
  vgc_call* calling = vgc_obj_ref_get(ctx,calling,vgc_call);
  vgc_subr* subr = vgc_obj_ref_get(calling,subr,vgc_subr);
  vgc_array* consts = vgc_obj_ref_get(subr,consts,vgc_array);
  if(vgc_obj_ref_check(consts,index))
    uabort("vm:constant error!");
  return consts->objs[index];
}

void bc_call(vcontext* ctx){
  vgc_heap* heap = ctx->heap;
  vgc_call* calling = vgc_obj_ref_get(ctx,calling,vgc_call);
  vslot slot;
  vgc_obj* obj;
  vgc_heap_stack_pop(heap,&slot);
  if(!vslot_is_ref(slot)){
    uabort("bc_call:not refrence can not execute!");
  }
  obj = vslot_ref_get(slot,vgc_obj);
  if(vgc_obj_typeof(obj,vgc_obj_type_subr)){
    vgc_subr* subr;
    vgc_call* call;
    usize_t base;
    subr = (vgc_subr*)obj;
    base = vgc_heap_stack_top_get(heap) + subr->locals_count;
    vgc_heap_stack_top_set(heap,base);
    vgc_heap_obj_push(heap,calling);
    vgc_heap_obj_push(heap,obj);
    call = vgc_call_new(heap,
			vgc_call_type_subr,
			base);
    if(!call){
      uabort("bc_call:subr out of memory!");
    }

    vgc_obj_ref_set(ctx,calling,call);
    vgc_heap_obj_push(heap,call);
  } else if(vgc_obj_typeof(obj,vgc_obj_type_cfun)){
    vgc_call* call;
    vgc_cfun* cfun;
    int has_retval;
    usize_t after_base;
    usize_t before_base = vgc_heap_stack_top_get(heap);
    vgc_heap_obj_push(heap,calling);
    vgc_heap_obj_push(heap,obj);
    call = vgc_call_new(heap,
			vgc_call_type_cfun,
			before_base);
    if(!call){
      uabort("bc_call:cfun out of memory!");
    }

    vgc_obj_ref_set(ctx,calling,call);
    cfun = vgc_obj_ref_get(call,cfun,vgc_cfun);
    has_retval = cfun->has_retval;
    (cfun->entry)(ctx);
    after_base = vgc_heap_stack_top_get(heap);
    if(has_retval){
      if (after_base - before_base != 1) {
	uabort("cfun claim return value but has no return!");
      }
    }else{
      if (after_base != before_base) {
	uabort("cfun claim has no return value!");
      }
    }
    call = vgc_obj_ref_get(ctx,calling,vgc_call);
    calling = vgc_obj_ref_get(call,caller,vgc_call);
    vgc_obj_ref_set(ctx,calling,calling);
  }else{
    uabort("bc_call:can not execute!");
  }
}

void bc_return(vcontext* ctx){
  vgc_heap* heap = ctx->heap;
  vgc_call* calling;
  vgc_call* called;
  vgc_subr* subr;
  vslot slot_val;
  int base;

  calling = vgc_obj_ref_get(ctx,calling,vgc_call);
  if(calling->call_type == vgc_call_type_subr){
    subr = vgc_obj_ref_get(calling,subr,vgc_subr);
    base = calling->base + subr->locals_count;
  }else{
    base = calling->base;
  }
  vgc_heap_stack_pop(heap,&slot_val);
  vgc_heap_stack_top_set(heap,base);
  vgc_heap_stack_push(heap,slot_val);
  called = vgc_obj_ref_get(calling,caller,vgc_call);
  vgc_obj_ref_set(ctx,calling,called);
}

void bc_return_void(vcontext* ctx){
  vgc_heap* heap = ctx->heap;
  vgc_call* calling;
  vgc_call* called;
  vgc_subr* subr;
  int base;
  
  calling = vgc_obj_ref_get(ctx,calling,vgc_call);
  if(calling->call_type == vgc_call_type_subr){
    subr = vgc_obj_ref_get(calling,subr,vgc_subr);
    base = calling->base + subr->locals_count;
  }else{
    base = calling->base;
  }
  vgc_heap_stack_top_set(heap,base);
  called = vgc_obj_ref_get(calling,caller,vgc_call);
  vgc_obj_ref_set(ctx,calling,called);
}

void bc_ref(vcontext* ctx,
	    usize_t index){
  vslot slot;
  vslot* slot_list;
  vgc_obj* obj;
  vgc_heap* heap = ctx->heap;
  vgc_heap_obj_pop(heap,obj,vgc_obj);
  if(!obj)
    uabort("vm:refrence null object!");
  if(vgc_obj_ref_check(obj,index))
    uabort("vm:ref obj error!");
  slot_list = vgc_obj_slot_list(obj);
  slot = slot_list[index];
  vgc_heap_stack_push(heap,slot);
}

void bc_set(vcontext* ctx,
	    usize_t   index){
  vslot slot_obj;
  vslot slot_val;
  vslot* slot_list;
  vgc_obj* obj;
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_pop(heap,&slot_obj);
  vgc_heap_stack_pop(heap,&slot_val);
  obj = vslot_ref_get(slot_obj,vgc_obj);
  if(!obj)
    uabort("vm:set null object!");
  if(vgc_obj_ref_check(obj,index))
    uabort("vm:set obj error!");
  slot_list = vgc_obj_slot_list(obj);
  slot_list[index] = slot_val;
}

#define CHECK_CURR_CALL							\
  do{									\
    vgc_call* __calling = vgc_obj_ref_get(ctx,calling,vgc_call);	\
    if(!__calling || vgc_call_is_cfun(__calling))			\
      goto context_exit;						\
  }while(0)

#define FETCH (*(vgc_obj_ref_get(ctx,calling,vgc_call)->pc)++)

#define NEXT (op=FETCH)

#define NEXT2 do{op=FETCH;op+=FETCH<<8;}while(0)

void vcontext_execute(vcontext* ctx){
  ulog("vcontext_execute");
  bc_call(ctx);
  while(1){
    int op;
    CHECK_CURR_CALL;
    NEXT;
    switch(op){
    case Bnop:
      ulog("Bnop %d",op);
      break;
    case Bload:
      NEXT;
      ulog("Bload %d",op);
      bc_push(ctx,bc_locals(ctx,op));
      break;
    case Bstore:
      NEXT;
      ulog("Bstore %d",op);
      bc_store(ctx,op);
      break;
    case Bpush:
      NEXT;
      ulog("Bpush %d",op);
      bc_push(ctx,bc_constant(ctx,op));
      break;
    case Btop:
      NEXT;
      op = (signed char)op;
      ulog("Btop %d",op);
      bc_top(ctx,op);
      break;
    case Bpop:
      ulog("Bpop");
      bc_pop(ctx);
      break;
    case Bjmp:
      NEXT2;
      ulog("Bjmp %d",op);
      bc_jmp(ctx,op);
      break;
    case Bjmpi:
      NEXT2;
      ulog("Bjmpi %d",op);
      bc_jmpi(ctx,op);
      break;
    case Beq:
      ulog("Beq");
      bc_eq(ctx);
      break;
    case Bgt:
      ulog("Bgt");
      bc_gt(ctx);
      break;
    case Blt:
      ulog("Blt");
      bc_lt(ctx);
      break;
    case Band:
      ulog("Band");
      bc_and(ctx);
      break;
    case Bor:
      ulog("Bor");
      bc_or(ctx);
      break;
    case Bnot:
      ulog("Bnot");
      bc_not(ctx);
      break;
    case Badd:
      ulog("Badd");
      bc_add(ctx);
      break;
    case Bsub:
      ulog("Bsub");
      bc_sub(ctx);
      break;
    case Bmul:
      ulog("Bmul");
      bc_mul(ctx);
      break;
    case Bdiv:
      ulog("Bdiv");
      bc_div(ctx);
      break;
    case Bcall:
      ulog("Bcall");
      bc_call(ctx);
      break;
    case Breturn:
      ulog("Breturn");
      bc_return(ctx);
      break;
    case Bretvoid:
      ulog("Bretvoid");
      bc_return_void(ctx);
      break;
    case Bref:
      NEXT2;
      ulog("Bref %d",op);
      bc_ref(ctx,op);
      break;
    case Bset:
      NEXT2;
      ulog("Bset %d",op);
      bc_set(ctx,op);
      break;
    default:
      uabort("bad byte code!");
    }
  }
 context_exit:
  return;
}
