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

int vinst_to_str(vcontext* ctx,ulist* insts){
  ulist*   header     = insts;
  ulist*   node       = insts;
  usize_t  inst_count = 0;
  usize_t  byte_count = 0;
  usize_t  length     = vinst_full_length(insts);
  vgc_string* str;
  if(vgc_string_new(ctx->heap,
		    length,
		    vgc_heap_area_active)){
    uabort("vinst_to_str:vgc_string new error!");
  }
  vgc_pop_obj(ctx->heap,str,vgc_string);
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
  vgc_push_obj(ctx->heap,str);
  return 0;
}

vsymbol* vsymbol_new(ustring* name,usize_t index){
  vsymbol* sym;
  unew(sym,sizeof(vsymbol),return NULL;);
  sym->name  = name;
  sym->index = index;
  return sym;
}

#define VCONTEXT_SYMTB_SIZE 17
#define VCONTEXT_CONSTS_SIZE 10

vcontext* vcontext_new(vgc_heap* heap){
  vcontext* ctx;
  uhash_table* symtb;
  vgc_array* consts;
  unew(ctx,
       sizeof(vcontext),
       uabort("vcontext_new:new error!"););
  symtb = uhash_table_new(VCONTEXT_SYMTB_SIZE);
  if(!symtb){
    uabort("vcontext_new:symtb new error!");
  }
  if(vgc_array_new(heap,VCONTEXT_CONSTS_SIZE,vgc_heap_area_static)){
    uabort("vcontext_new:consts new error!");
  }
  vgc_pop_obj(heap,consts,vgc_array);
  ctx->heap = heap;
  ctx->calling = NULL;
  ctx->symtb = symtb;
  ctx->consts = consts;
  return ctx;
}

void bc_top(vcontext* ctx,usize_t top){
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_top_set(heap,top);
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
  vgc_call* calling = ctx->calling;
  vgc_subr* subr = vgc_obj_ref_get(calling,subr,vgc_subr);
  usize_t count = subr->params_count + subr->locals_count;
  usize_t base = calling->base;
  vslot slot;
  if(index >= count)
    uabort("vm:local varable error!");
  slot = vgc_heap_stack_get(ctx->heap,base - count + index);
  return slot;
}

void bc_store(vcontext* ctx,
	      usize_t index){
  vslot slot;
  vgc_heap* heap = ctx->heap;
  vgc_call* calling = ctx->calling;
  vgc_subr* subr = vgc_obj_ref_get(calling,subr,vgc_subr);
  usize_t count = subr->params_count + subr->locals_count;
  usize_t base = calling->base;
  if(index < base - count || index >= base)
    uabort("vm:local varable error!");
  vgc_heap_stack_pop(heap,&slot);
  vgc_heap_stack_set(heap,index,slot);
}

void bc_ref(vcontext* ctx,
	    usize_t index){
  vslot slot;
  vslot* slot_list;
  vgc_obj* obj;
  vgc_heap* heap = ctx->heap;
  vgc_pop_obj(heap,obj,vgc_obj);
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

void bc_eq(vcontext* ctx){
  vslot slot1;
  vslot slot2;
  vslot bool;
  vgc_heap_stack_pop(ctx->heap,&slot1);
  vgc_heap_stack_pop(ctx->heap,&slot2);
  
  if(vslot_is_num(slot1) &&
     vslot_is_num(slot2)){
    bool = vslot_num_eq(slot1,slot2);
  }else if(vslot_is_ref(slot1) &&
	   vslot_is_ref(slot2)){
    bool = vslot_ref_eq(slot1,slot2);
  }else{
    vgc_heap_stack_log(ctx->heap);
    uabort("vm:eq type error!");
  }
  vgc_heap_stack_push(ctx->heap,bool);
}

void bc_jmp(vcontext* ctx,
	    usize_t offset){
  vgc_call* calling = ctx->calling;
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
  if(vslot_is_bool(slot) && VTRUEP(slot)){
    bc_jmp(ctx,offset);
  }
}

vslot bc_constant(vcontext* ctx,
		  usize_t index){
  vgc_call* calling = ctx->calling;
  vgc_subr* subr = vgc_obj_ref_get(calling,subr,vgc_subr);
  vgc_array* consts = vgc_obj_ref_get(subr,consts,vgc_array);
  if(vgc_obj_ref_check(consts,index))
    uabort("vm:constant error!");
  return consts->objs[index];
}

void bc_call(vcontext* ctx){
  vgc_heap* heap = ctx->heap;
  vslot slot;
  vgc_obj* obj;
  vgc_heap_stack_pop(heap,&slot);
  if(!vslot_is_ref(slot)){
    uabort("bc_call:not refrence can not execute!");
  }
  obj = vslot_ref_get(slot,vgc_obj);
  if(vgc_obj_typeof(obj,vgc_obj_type_subr)){
    vgc_call* call;
    usize_t base = vgc_heap_stack_top_get(heap);
    vgc_push_obj(heap,ctx->calling);
    vgc_push_obj(heap,obj);
    if(vgc_call_new(heap,
		    vgc_call_type_subr,
		    base)){
      uabort("bc_call:subr out of memory!");
    }
    vgc_pop_obj(heap,call,vgc_call);
    ctx->calling = call;
    vgc_push_obj(heap,call);
  } else if(vgc_obj_typeof(obj,vgc_obj_type_cfun)){
    vgc_call* call;
    vgc_cfun* cfun;
    usize_t base = vgc_heap_stack_top_get(heap);
    ulog("bc_call:call subr");
    vgc_push_obj(heap,ctx->calling);
    vgc_push_obj(heap,obj);
    if(vgc_call_new(heap,
		    vgc_call_type_cfun,
		    base)){
      uabort("bc_call:cfun out of memory!");
    }
    vgc_pop_obj(heap,call,vgc_call);
    ctx->calling = call;
    cfun = vgc_obj_ref_get(call,cfun,vgc_cfun);
    (cfun->entry)(ctx);
    vgc_heap_stack_top_set(heap,base);
  }else{
    uabort("bc_call:can not execute!");
  }
}

void bc_return(vcontext* ctx){
  vgc_heap* heap = ctx->heap;
  vgc_call* calling = ctx->calling;
  vslot slot_val;
  vgc_heap_stack_pop(heap,&slot_val);
  vgc_heap_stack_top_set(heap,calling->base);
  vgc_heap_stack_push(heap,slot_val);
  ctx->calling = vgc_obj_ref_get(calling,caller,vgc_call);
}

void bc_return_void(vcontext* ctx){
  vgc_heap* heap = ctx->heap;
  vgc_call* calling = ctx->calling;
  vslot slot;
  vgc_heap_stack_top_set(heap,calling->base);
  vslot_null_set(slot);
  vgc_heap_stack_push(heap,slot);
  ctx->calling = vgc_obj_ref_get(calling,caller,vgc_call);
}

#define CHECK_CURR_CALL						 \
  if(!ctx->calling || vgc_call_is_cfun(ctx->calling))		 \
    goto context_exit

#define FETCH (*((ctx->calling)->pc)++)

#define NEXT (op=FETCH)

#define NEXT2 do{op=FETCH;op+=FETCH<<8;}while(0)

void vcontext_execute(vcontext* ctx){
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
