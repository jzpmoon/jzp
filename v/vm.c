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
  vgc_str* str        = vgc_str_new(heap,area_active,length);
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

vsymbol* vsymbol_new(ustring* name,usize_t index){
  vsymbol* sym;
  unew(sym,sizeof(vsymbol),return NULL;);
  sym->name  = name;
  sym->index = index;
  return sym;
}

vcontext*
vcontext_new(struct _vm* vm,usize_t stack_size){
  vgc_heap* heap = vm->heap;
  vslot* stack;
  vcontext* ctx;
  stack = vgc_stack_new(heap,NULL,stack_size,area_static);
  if(!stack) return NULL;
  ctx = vgc_heap_obj_new(heap,
			 stack,
			 vcontext,
			 2,
			 gc_context,
			 area_static);
  if(ctx){
    ctx->vm        = vm;
    ctx->stack     = *stack;
    ctx->curr_call = VSLOT_NULL;
    vgc_obj_flip((vgc_obj*)ctx);
  }
  return ctx;
}

vslot* vcontext_args(vcontext* ctx,usize_t index){
  vslot* curr_call = &ctx->curr_call;
  vslot* locals = vslotp_ref_get(curr_call,vgc_call,locals);
  vslot* arg = vslotp_ref_get(locals,vgc_stack,objs[index]);
  return arg;
}

vm* vm_new(usize_t static_size,
	   usize_t active_size,
	   usize_t stack_size,
	   usize_t consts_size){
  vgc_heap*    heap;
  vcontext*    ctx;
  vgc_stack*   consts;
  uhash_table* symtb;
  vm*          vm;

  heap = vgc_heap_new(static_size,active_size);
  if(!heap){
    uabort("vm:create heap error!");
  }

  consts = vgc_stack_new(heap,NULL,consts_size,area_static);
  if(!consts){
    uabort("vm:create consts error!");
  }

  symtb = uhash_table_new(VM_SYMTB_SIZE);
  if(!symtb){
    uabort("vm:create symtb error!");
  }
  
  unew(vm,
       sizeof(vm),
       uabort("vm:create vm error!"););
  
  vm->heap    = heap;
  vm->symtb   = symtb;
  vm->consts  = consts;
  
  ctx = vcontext_new(vm,stack_size);
  if(!ctx){
    uabort("vm:create context error!");
  }

  vm->context = ctx;
  return vm;
}

void* vm_symtb_key_put(void* key){
  vsymbol* sym = key;
  vsymbol* new_sym;
  new_sym = vsymbol_new(sym->name,sym->index);
  return new_sym;
}

int vm_symtb_key_comp(void* k1,void* k2){
  vsymbol* sym1 = k1;
  vsymbol* sym2 = k2;
  return sym1->name - sym2->name;
}

int vm_obj_put(vm* vm,ustring* name,vgc_obj* obj){
  vsymbol sym;
  vsymbol* new_sym;
  uhash_table* symtb = vm->symtb;
  vgc_stack*   consts = vm->consts;
  vslot slot_obj;
  vslot_ref_set(slot_obj,obj);
  int index = vgc_stack_push(consts,slot_obj);
  if(index < 0){
    return -1;
  }
  sym = vsymbol_init(name,index);
  new_sym = uhash_table_put(symtb,
			    name->hash_code % VM_SYMTB_SIZE,
			    &sym,
			    vm_symtb_key_put,
			    vm_symtb_key_comp);
  if(!new_sym){
    return -1;
  }else{
    return index;
  }
}

void* vm_symtb_key_get(void* key){
  return key;
}

int vm_obj_get(vm* vm,ustring* name){
  uhash_table* symtb = vm->symtb;
  vsymbol* sym = uhash_table_get(symtb,
				 name->hash_code % VM_SYMTB_SIZE,
				 name,
				 vm_symtb_key_get,
				 vm_symtb_key_comp);
  if(!sym){
    return -1;
  }
  return sym->index;
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
			 usize_t para_len,
			 usize_t local_len){
  usize_t locals_len =
    para_len + local_len;
  usize_t i = 0;
  vgc_stack* locals =
    vgc_stack_new(ctx->vm->heap,locals_len);
  if(!locals)
    uabort("out of memory!");
  while(i < para_len){
    vslot para = bc_pop(ctx);
    vgc_stack_push(locals,para);
    i++;
  }
  vgc_obj_flip(locals);
  return locals;
}

void bc_call(vcontext* ctx){
  vgc_heap* heap      = ctx->vm->heap;
  vgc_stack* stack    = (vgc_stack*)
    vslot_ref_get(ctx->stack);
  vgc_call* curr_call = (vgc_call*)
    vslot_ref_get(ctx->curr_call);
  vslot slot          = bc_pop(ctx);
  vgc_obj* obj        = vslot_ref_get(slot);
  if(VGCTYPEOF(obj,gc_subr)){
    vgc_subr* subr    = (vgc_subr*)obj;
    vgc_stack* locals =
      bc_locals_new(ctx,subr->para_len,subr->local_len);
    vgc_call* call    =
      vgc_call_new(heap,
		   stack->top,
		   subr,
		   NULL,
		   locals,
		   curr_call);
    vslot slot_call;
    vslot_ref_set(slot_call,call);
    if(!call)
      uabort("out of memory!");
    bc_push(ctx,slot_call);
    ctx->curr_call = slot_call;
  } else if(VGCTYPEOF(obj,gc_cfun)){
    vgc_cfun* cfun = (vgc_cfun*)obj;
    vgc_stack* locals =
      bc_locals_new(ctx,cfun->para_len,cfun->local_len);
    vgc_call* call    =
      vgc_call_new(heap,
		   stack->top,
		   NULL,
		   cfun,
		   locals,
		   curr_call);
    vslot slot_call;
    vslot_ref_set(slot_call,call);
    if(!call)
      uabort("out of memory!");
    bc_push(ctx,slot_call);
    cfun->entry(ctx);
    bc_top(ctx,call->base);
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
		      vgc_obj* entry){
  vslot slot_entry;
  vslot_ref_set(slot_entry,entry);
  bc_push(ctx,slot_entry);
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
