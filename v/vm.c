#include <stddef.h>
#include "ualloc.h"
#include "uerror.h"
#include "vm.h"

#define VCONTEXT_OBJTB_SIZE 17
#define VCONTEXT_SYMTB_SIZE 17
#define VCONTEXT_STRTB_SIZE 17
#define VCONTEXT_CONSTS_SIZE 10

vcontext* vcontext_new(vgc_heap* heap){
  vcontext* ctx;
  uhash_table* objtb;
  ustring_table* symtb;
  ustring_table* strtb;
  vgc_array* consts;

  unew(ctx,
       sizeof(vcontext),
       uabort("vcontext_new:new error!"););
  
  objtb = uhash_table_new(VCONTEXT_OBJTB_SIZE);
  if(!objtb){
    uabort("vcontext_new:objtb new error!");
  }
  
  symtb = uhash_table_new(VCONTEXT_SYMTB_SIZE);
  if(!symtb){
    uabort("vcontext_new:symtb new error!");
  }

  strtb = uhash_table_new(VCONTEXT_STRTB_SIZE);
  if(!strtb){
    uabort("vcontext_new:strtb new error!");
  }
  consts = vgc_array_new(heap,
			 VCONTEXT_CONSTS_SIZE,
			 vgc_heap_area_static);
  if(!consts){
    uabort("vcontext_new:consts new error!");
  }

  ctx->heap = heap;
  ctx->calling = NULL;
  ctx->objtb = objtb;
  ctx->symtb = symtb;
  ctx->strtb = strtb;
  ctx->consts = consts;
  return ctx;
}

vsymbol* vsymbol_new(ustring* name,vslot slot){
  vsymbol* symbol = ualloc(sizeof(vsymbol));
  if(symbol){
    symbol->name = name;
    symbol->slot = slot;
  }
  return symbol;
}

static void* vobjtb_key_put(void* key){
  vsymbol* symbol = (void*)key;
  vsymbol* new_symbol = vsymbol_new(symbol->name,symbol->slot);
  if(!new_symbol){
    uabort("vobjtb key put error!");
  }
  return new_symbol;
}

static int vobjtb_key_comp(void* k1,void* k2){
  vsymbol* sym1 = (vsymbol*)k1;
  vsymbol* sym2 = (vsymbol*)k2;
  return (sym1->name - sym2->name);
}

vsymbol* vcontext_obj_put(vcontext* ctx,ustring* name,vslot obj){
  vsymbol symbol = (vsymbol){name,obj};
  vsymbol* new_symbol = 
    uhash_table_put(ctx->objtb,
		    name->hash_code % VCONTEXT_OBJTB_SIZE,
		    (void*)&symbol,
		    vobjtb_key_put,
		    vobjtb_key_comp);
  return new_symbol;
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
    call = vgc_call_new(heap,
			vgc_call_type_subr,
			base);
    if(!call){
      uabort("bc_call:subr out of memory!");
    }
    ctx->calling = call;
    vgc_push_obj(heap,call);
  } else if(vgc_obj_typeof(obj,vgc_obj_type_cfun)){
    vgc_call* call;
    vgc_cfun* cfun;
    usize_t base = vgc_heap_stack_top_get(heap);
    vgc_push_obj(heap,ctx->calling);
    vgc_push_obj(heap,obj);
    call = vgc_call_new(heap,
			vgc_call_type_cfun,
			base);
    if(!call){
      uabort("bc_call:cfun out of memory!");
    }
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
    case Bnop:
      ulog1("Bnop %d",op);
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
    case Bpush:
      NEXT;
      ulog1("Bpush %d",op);
      bc_push(ctx,bc_constant(ctx,op));
      break;
    case Bpop:
      ulog("Bpop");
      bc_pop(ctx);
      break;
    case Bjmp:
      NEXT2;
      ulog1("Bjmp %d",op);
      bc_jmp(ctx,op);
      break;
    case Bjmpi:
      NEXT2;
      ulog1("Bjmpi %d",op);
      bc_jmpi(ctx,op);
      break;
    case Beq:
      ulog("Beq");
      bc_eq(ctx);
      break;
    case Bgt:
      ulog("Bgt");
      break;
    case Blt:
      ulog("Blt");
      break;
    case Band:
      ulog("Band");
      break;
    case Bor:
      ulog("Bor");
      break;
    case Bnot:
      ulog("Bnot");
      break;
    case Badd:
      ulog("Badd");
      bc_add(ctx);
      break;
    case Bsub:
      ulog("Bsub");
      break;
    case Bmul:
      ulog("Bmul");
      break;
    case Bdiv:
      ulog("Bdiv");
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
      ulog1("Bref %d",op);
      bc_ref(ctx,op);
      break;
    case Bset:
      NEXT2;
      ulog1("Bset %d",op);
      bc_set(ctx,op);
      break;
    default:
      uabort("bad byte code!");
    }
  }
 context_exit:
  return;
}
