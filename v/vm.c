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

vslot*
vinst_to_str(vcontext* ctx,ulist* insts){
  ulist*   header     = insts;
  ulist*   node       = insts;
  usize_t  inst_count = 0;
  usize_t  byte_count = 0;
  usize_t  length     = vinst_full_length(insts);
  vslot*   slotp      = vgc_str_new(ctx->m->heap,
				    ctx->cache,
				    length,
				    area_active);
  vgc_str* str;
  if(!slotp) return NULL;
  str = vslot_ref_get(*slotp);
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
  return slotp;
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
  symtb = uhash_table(VCONTEXT_SYMTB_SIZE);
  if(!symtb){
    uabort("vcontext_new:symtb new error!");
  }
  if(vgc_array_new(heap,VCONTEXT_CONSTS_SIZE)){
    uabort("vcontext_new:consts new error!");
  }
  vgc_pop_obj(heap,consts,vgc_array);
  ctx->heap = heap;
  ctx->calling = NULL;
  ctx->symtb = symtb;
  ctx->consts = consts;
  return ctx;
}

void* vcontext_symtb_key_put(void* key){
  vsymbol* sym = key;
  vsymbol* new_sym;
  new_sym = vsymbol_new(sym->name,sym->index);
  return new_sym;
}

int vcontext_symtb_key_comp(void* k1,void* k2){
  vsymbol* sym1 = k1;
  vsymbol* sym2 = k2;
  return sym1->name - sym2->name;
}

int vcontext_obj_put(vcontext* ctx,char* name,vslot* obj){
  vsymbol sym;
  vsymbol* new_sym;
  uhash_table* symtb = ctx->symtb;
  vgc_stack* consts = ctx->consts;
  ustring* str;
  int index;
  str = vstrtb_put(name,-1);
  if(!str){
    return -1;
  }
  index = vgc_stack_push(consts,*obj);
  if(index < 0){
    return -1;
  }
  sym = vsymbol_init(str,index);
  new_sym = uhash_table_put(ctx->symtb,
			    str->hash_code % VM_SYMTB_SIZE,
			    &sym,
			    vcontext_symtb_key_put,
			    vcontext_symtb_key_comp);
  if(!new_sym){
    return -1;
  }else{
    return index;
  }
}

void* vm_symtb_key_get(void* key){
  return key;
}

vslot* vcontext_obj_get(vcontext* ctx,char* name){
  int index = vm_obj_get_index(m,name);
  if(vgc_obj_ref_check(m->consts,index)){
    return &m->consts->objs[index];
  }else{
    return NULL;
  }
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
  vgc_subr* subr = vslot_ref_get(calling->subr);
  usize_t count = subr->params_count + subr->locals_count;
  usize_t base = calling->base;
  if(index < base - count || index >= base)
    uabort("vm:local varable error!");
  return vgc_heap_stack_get(ctx->heap,index);
}

void bc_store(vcontext* ctx,
	      usize_t index){
  vslot slot;
  vgc_heap* heap = ctx->heap;
  vgc_call* calling = ctx->calling;
  vgc_subr* subr = vslot_ref_get(calling->subr);
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
  vgc_obj* obj;
  vgc_heap* heap = ctx->heap;
  vgc_pop_obj(heap,obj,vgc_obj);
  if(!obj)
    uabort("vm:refrence null object!");
  if(!vgc_obj_ref_check(obj,index))
    uabort("vm:ref obj error!");
  slot = vgc_obj_ref_list(obj)[index];
  vgc_heap_stack_push(heap,slot);
}

void bc_set(vcontext* ctx,
	    usize_t   index){
  vslot slot_obj;
  vslot slot_val;
  vgc_obj* obj;
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_pop(heap,&slot_obj);
  vgc_heap_stack_pop(heap,&slot_val);
  obj = vslot_ref_get(slot_obj);
  if(!obj)
    uabort("vm:set null object!");
  if(!vgc_obj_ref_check(obj,index))
    uabort("vm:set obj error!");
  vgc_obj_ref_list(obj)[index]=slot_val;
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
  vgc_heap_stack_pop(heap,&slot1);
  vgc_heap_stack_pop(heap,&slot2);
  if(vslot_is_num(slot1) &&
     vslot_is_num(slot2)){
    bool = vslot_num_eq(slot1,slot2);
  }else if(vslot_is_ref(slot1) &&
	   vslot_is_ref(slot2)){
    bool = vslot_ref_eq(slot1,slot2);
  }else{
    uabort("vm:eq type error!");
  }
  vgc_heap_stack(heap,bool);
}

void bc_jmp(vcontext* ctx,
	    usize_t   offset){
  vgc_call* calling = ctx->calling;
  vgc_subr* subr = vslot_ref_get(calling->subr);
  vgc_string* bytecode = vslot_ref_get(subr->bytecode);
  if(!vgc_str_bound_check(bc,offset))
    uabort("vm:jmp error!");
  calling->pc = bytecode->u.b + offset;
}

void bc_jmpi(vcontext* ctx,
	     usize_t   offset){
  vslot slot;
  vgc_heap* heap = ctx->heap;
  vgc_heap_stack_pop(heap,slot);
  if(vslot_is_bool(slot) && VTRUEP(slot))
    bc_jmp(ctx,offset);
}

vslot bc_constant(vcontext* ctx,
		  usize_t index){
  vgc_call* calling = ctx->calling;
  vgc_subr* subr = vslot_ref_get(calling->subr);
  vgc_array* consts = vslot_ref_get(subr->consts);
  if(!vgc_obj_ref_check(consts,index))
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
  obj = vslot_ref_get(slot);
  if(vgc_obj_typeof(obj,gc_subr)){
    vgc_call* call;
    usize_t base = vgc_heap_stack_top_get(heap);
    vgc_push_obj(heap,ctx-calling);
    vgc_push_obj(heap,obj);
    if(vgc_call_new(heap,
		    vgc_call_type_subr,
		    base)){
      uabort("bc_call:subr out of memory!");
    }
    call = vslot_ref_get(vgc_heap_stack_get(heap,-1));
    ctx->calling = call;
  } else if(vgc_obj_typeof(obj,gc_cfun)){
    vgc_call* call;
    vgc_cfun* cfun;
    usize_t base = vgc_heap_stack_top_get(heap);
    vgc_push_obj(heap,ctx-calling);
    vgc_push_obj(heap,obj);
    if(vgc_call_new(heap,
		    vgc_call_type_cfun,
		    base)){
      uabort("bc_call:cfun out of memory!");
    }
    call = vslot_ref_get(vgc_heap_stack_get(heap,-1));
    ctx->calling = call;
    cfun = vslot_ref_get(call->cfun);
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
  ctx->calling = calling->caller;
}

void bc_return_void(vcontext* ctx){
  vgc_heap* heap = ctx->heap;
  vgc_call* calling = ctx->calling;
  vgc_heap_stack_top_set(heap,calling->base);
  vgc_heap_stack_push(heap,VSLOT_NULL);
  ctx->calling = calling->caller;
}

#define CHECK_CURR_CALL						 \
  if(!ctx->calling || vgc_call_is_cfun(ctx->calling))		 \
    goto context_exit

#define FETCH (*(((vgc_call*)vslot_ref_get(ctx->curr_call))->pc)++)

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
