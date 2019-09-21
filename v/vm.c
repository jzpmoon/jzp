#include <stddef.h>
#include "ualloc.h"
#include "uerror.h"
#include "vm.h"

vinst*
vinst_new(usize_t code,usize_t operand){
  vinst* inst;
  unew(inst,sizeof(vinst),uabort("vinst:new error!"););
  inst->code    = code;
  inst->operand = operand;
  return inst;
}

usize_t
vinst_full_length(ulist* insts){
  ulist*  header = insts;
  ulist*  node   = insts;
  usize_t length = 0;
  do{
    vinst* inst = node->value;
    switch(inst->code){
#define DF(code,name,value,len)			\
      case (code):length+=len;break;
      VBYTE_CODE
#undef DF
    }
  } while((node=node->next)!=header);
  return length;
}

usize_t
vinst_byte_length(ulist* insts,usize_t offset){
  usize_t i      = 0;
  usize_t length = 0;
  ulist*  node   = insts;
  while(i < offset){
    vinst* inst = node->value;
    switch(inst->code){
#define DF(code,name,value,len)			\
      case (code):length+=len;break;
      VBYTE_CODE
#undef DF
    }
    node = node->next;
    i++;
  }
  return length;
}

vgc_str*
vinst_to_str(vgc_heap* heap,ulist* insts){
  ulist*   header     = insts;
  ulist*   node       = insts;
  usize_t  inst_count = 0;
  usize_t  byte_count = 0;
  usize_t  length     = vinst_full_length(insts);
  vgc_str* str        = vgc_str_new(heap,length);
  if(!str) return NULL;
  do{
    vinst* inst=node->value;
    switch(inst->code){
#define DF(ocode,name,value,len)					\
      case (ocode):{							\
	int i = 0;							\
	usize_t operand;						\
	if(ocode == Bjmp || ocode == Bjmpi){				\
	  operand = vinst_byte_length(header,				\
				      inst_count + inst->operand);	\
	}else{								\
	  operand = inst->operand;					\
	}								\
	str->u.b[byte_count++] = inst->code;				\
	while(i<len-1&&i<sizeof(usize_t)){				\
	  str->u.b[byte_count++] = operand>>(8*i);			\
	  i++;								\
	}								\
	inst_count++;							\
	break;								\
      }						
      VBYTE_CODE				
#undef DF
    }
  }while((node = node->next) != header);
  return str;
}

vcontext*
vcontext_new(vgc_heap* heap,usize_t stack_size){
  vgc_stack* stack;
  vcontext* ctx;
  stack=vgc_stack_new(heap,stack_size);
  if(!stack) return NULL;
  ctx = (vcontext*)vgc_heap_obj_new(heap,
				    vcontext,
				    2,
				    gc_context,
				    area_static);
  if(ctx){
    ctx->heap = heap;
    vslot_ref_set(ctx->stack,stack);
    vslot_ref_set(ctx->curr_call,NULL);
    vgc_obj_flip((vgc_obj*)ctx);
  }
  return ctx;
}

void bc_top(vcontext* ctx,usize_t top){
  vgc_stack* stack = (vgc_stack*)
    vslot_ref_get(ctx->stack);
  if(!vgc_obj_ref_check(stack,top))
    uabort("vm:stack top error!");
  stack->top=top;
}

void bc_push(vcontext* ctx,
	     vslot     slot){
  vgc_stack* stack = (vgc_stack*)
    vslot_ref_get(ctx->stack);
  if(vgc_obj_is_full(stack))
    uabort("stack overflow!");
  vgc_stack_push(stack,slot);
}

vslot bc_pop(vcontext* ctx){
  vgc_stack* stack = (vgc_stack*)
    vslot_ref_get(ctx->stack);
  if(vgc_obj_is_empty(stack))
    uabort("vm:stack empty!");
  return vgc_stack_pop(stack);
}

vslot bc_locals(vcontext* ctx,
		usize_t   index){
  vgc_call* curr_call = (vgc_call*)
    vslot_ref_get(ctx->curr_call);
  vgc_stack* locals   = (vgc_stack*)
    vslot_ref_get(curr_call->locals);
  if(!vgc_obj_ref_check(locals,index))
    uabort("vm:local varable error!");
  return locals->objs[index];
}

void bc_store(vcontext* ctx,
	      usize_t index){
  vslot slot          = bc_pop(ctx);
  vgc_call* curr_call = (vgc_call*)
    vslot_ref_get(ctx->curr_call);
  vgc_stack* locals   = (vgc_stack*)
    vslot_ref_get(curr_call->locals);
  if(!vgc_obj_ref_check(locals,index))
    uabort("vm:local varable error!");
  locals->objs[index] = slot;
}

void bc_ref(vcontext* ctx,
	    usize_t index){
  vslot slot   = bc_pop(ctx);
  vgc_obj* obj = vslot_ref_get(slot);
  if(!obj)
    uabort("vm:refrence null object!");
  if(!vgc_obj_ref_check(obj,index))
    uabort("vm:ref obj error!");
  bc_push(ctx,vgc_obj_ref_list(obj)[index]);
}

void bc_set(vcontext* ctx,
	    usize_t   index){
  vslot slot_obj = bc_pop(ctx);
  vslot slot_val = bc_pop(ctx);
  vgc_obj* obj   = vslot_ref_get(slot_obj);
  if(!obj)
    uabort("vm:set null object!");
  if(!vgc_obj_ref_check(obj,index))
    uabort("vm:set obj error!");
  vgc_obj_ref_list(obj)[index]=slot_val;
}

void bc_add(vcontext* ctx){
  vslot slot1 = bc_pop(ctx);
  vslot slot2 = bc_pop(ctx);
  vslot num;
  if(!vslot_is_num(slot1) ||
     !vslot_is_num(slot2)){
    uabort("vm:add not a number!");
  }
  num = vslot_num_add(slot1,slot2);
  bc_push(ctx,num);
}

void bc_eq(vcontext* ctx){
  vslot slot1 = bc_pop(ctx);
  vslot slot2 = bc_pop(ctx);
  vslot bool;
  if(vslot_is_num(slot1) &&
     vslot_is_num(slot2)){
    bool = vslot_num_eq(slot1,slot2);
  }else if(vslot_is_ref(slot1) &&
	   vslot_is_ref(slot2)){
    bool = vslot_ref_eq(slot1,slot2);
  }else{
    uabort("vm:eq type error!");
  }
  bc_push(ctx,bool);
}

void bc_jmp(vcontext* ctx,
	    usize_t   offset){
  vgc_call* curr_call = (vgc_call*)
    vslot_ref_get(ctx->curr_call);
  vgc_subr* subr      = (vgc_subr*)
    vslot_ref_get(curr_call->subr);
  vgc_str* bc         = (vgc_str*)
    vslot_ref_get(subr->bc);
  if(!vgc_str_bound_check(bc,offset))
    uabort("vm:jmp error!");
  curr_call->pc=bc->u.b+offset;
}

void bc_jmpi(vcontext* ctx,
	     usize_t   offset){
  vslot slot = bc_pop(ctx);
  if(vslot_is_bool(slot) && VTRUEP(slot))
    bc_jmp(ctx,offset);
}

vslot bc_constant(vcontext* ctx,
		  usize_t   index){
  vgc_call* curr_call = (vgc_call*)
    vslot_ref_get(ctx->curr_call);
  vgc_subr* subr      = (vgc_subr*)
    vslot_ref_get(curr_call->subr);
  vgc_stack* consts   = (vgc_stack*)
    vslot_ref_get(subr->consts);
  if(!vgc_obj_ref_check(consts,index))
    uabort("vm:constant error!");
  return consts->objs[index];
}

vgc_stack* bc_locals_new(vcontext* ctx,
			 vgc_subr* subr){
  usize_t locals_len =
    subr->para_len + subr->local_len;
  usize_t i = 0;
  vgc_stack* locals =
    vgc_stack_new(ctx->heap,locals_len);
  if(!locals)
    uabort("out of memory!");
  while(i < subr->para_len){
    vslot para = bc_pop(ctx);
    vgc_stack_push(locals,para);
    i++;
  }
  vgc_obj_flip(locals);
  return locals;
}

void bc_call(vcontext* ctx){
  vgc_heap* heap      = ctx->heap;
  vgc_stack* stack    = (vgc_stack*)
    vslot_ref_get(ctx->stack);
  vgc_call* curr_call = (vgc_call*)
    vslot_ref_get(ctx->curr_call);
  vslot slot          = bc_pop(ctx);
  vgc_obj* obj        = vslot_ref_get(slot);
  if(VGCTYPEOF(obj,gc_subr)){
    vgc_subr* subr    = (vgc_subr*)obj;
    vgc_stack* locals =
      bc_locals_new(ctx,subr);
    vgc_call* call    =
      vgc_call_new(heap,
		   stack->top,
		   subr,
		   locals,
		   curr_call);
    vslot slot_call;
    vslot_ref_set(slot_call,call);
    if(!call)
      uabort("out of memory!");
    bc_push(ctx,slot_call);
    ctx->curr_call = slot_call;
  }else{
    uabort("cant't execute!");
  }
}

void bc_return(vcontext* ctx){
  vgc_call* curr_call = (vgc_call*)
    vslot_ref_get(ctx->curr_call);
  vslot slot_val      = bc_pop(ctx);
  bc_top(ctx,curr_call->base);
  bc_push(ctx,slot_val);
  ctx->curr_call = curr_call->caller;
}

void bc_return_void(vcontext* ctx){
  vgc_call* curr_call = (vgc_call*)
    vslot_ref_get(ctx->curr_call);
  bc_top(ctx,curr_call->base);
  bc_push(ctx,VSLOT_NULL);
  ctx->curr_call = curr_call->caller;
}

#define CHECK_CURR_CALL \
  if(!vslot_ref_get(ctx->curr_call)) goto context_exit

#define FETCH (*(((vgc_call*)vslot_ref_get(ctx->curr_call))->pc)++)

#define NEXT (op=FETCH)

#define NEXT2 do{op=FETCH;op+=FETCH<<8;}while(0)

void vcontext_execute(vcontext* ctx,
		      vgc_subr* subr){
  vslot slot_subr;
  vslot_ref_set(slot_subr,subr);
  bc_push(ctx,slot_subr);
  bc_call(ctx);
  while(1){
    int op;
    CHECK_CURR_CALL;
    NEXT;
    switch(op){
    case Bpush:
      NEXT;
      ulog1("Bpush %d",op);
      bc_push(ctx,bc_constant(ctx,op));
      break;
    case Bpop:
      ulog("Bpop");
      bc_pop(ctx);
      break;
    case Bload:
      NEXT;
      ulog1("Bload %d",op);
      bc_push(ctx,bc_locals(ctx,op));
      break;
    case Bstore:
      NEXT;
      ulog1("Bstore %d",op);
      bc_store(ctx,op);
      break;
    case Bref:
      NEXT2;
      ulog1("Bref %d",op);
      bc_ref(ctx,op);
      break;
    case Bset:
      NEXT2;
      ulog1("Bset %d",op);
      bc_set(ctx,op);
      break;
    case Badd:
      ulog("Badd");
      bc_add(ctx);
      break;
    case Beq:
      ulog("Beq");
      bc_eq(ctx);
      break;
    case Bjmpi:
      NEXT2;
      ulog1("Bjmpi %d",op);
      bc_jmpi(ctx,op);
      break;
    case Bjmp:
      NEXT2;
      ulog1("Bjmp %d",op);
      bc_jmp(ctx,op);
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
    default:
      uabort("bad byte code!");
    }
  }
 context_exit:
  return;
}
