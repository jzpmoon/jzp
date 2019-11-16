#include <stddef.h>
#include "ualloc.h"
#include "uerror.h"
#include "vvm.h"

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
  vslot*   slotp      = vgc_str_new(ctx->vm->heap,
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

#define VCTX_CACHE_SIZE 5

vcontext*
vcontext_new(struct _vvm* vm,usize_t stack_size){
  vgc_heap* heap = vm->heap;
  vgc_stack* stack;
  vgc_stack* cache;
  vcontext* ctx;
  stack = vgc_stack_new(heap,NULL,stack_size,area_static);
  if(!stack) return NULL;
  cache = vgc_stack_new(heap,NULL,VCTX_CACHE_SIZE,area_static);
  if(!cache) return NULL;
  ctx = vgc_heap_obj_new(heap,
			 NULL,
			 vcontext,
			 3,
			 gc_context,
			 area_static);
  if(ctx){
    ctx->vm        = vm;
    ctx->cache     = cache;
    vslot_ref_set(ctx->stack,stack);
    ctx->curr_call = VSLOT_NULL;
    ctx->retval    = VSLOT_NULL;
    vgc_obj_flip((vgc_obj*)ctx);
  }
  return ctx;
}

vslot* vcontext_cache(vcontext* ctx,vslot slot){
  if(vgc_stack_push(ctx->cache,slot) == -1){
    uabort("vcontext cache overflow!");
  }
  return vgc_stack_top_ref(ctx->cache);
}

void vcontext_cache_clean(vcontext* ctx){
  vgc_stack* cache = ctx->cache;
  cache->top = 0;
}

vslot* vcontext_args_get(vcontext* ctx,usize_t index){
  vgc_call* curr_call = vslot_ref_get(ctx->curr_call);
  vgc_stack* locals = vslot_ref_get(curr_call->locals);
  if(!vgc_obj_ref_check(locals,index)){
    return NULL;
  }
  return &locals->objs[index];
}

int vcontext_call_return(vcontext* ctx,vslot* slotp){
  if(slotp){
    ctx->retval = *slotp;
  }else{
    ctx->retval = VSLOT_NULL;
  }
  return 0;
}

vvm* vvm_new(usize_t static_size,
	     usize_t active_size,
	     usize_t stack_size,
	     usize_t consts_size){
  vgc_heap*    heap;
  vcontext*    ctx;
  vgc_stack*   consts;
  uhash_table* symtb;
  vvm*          vm;

  heap = vgc_heap_new(static_size,active_size);
  if(!heap){
    uabort("vvm:create heap error!");
  }

  consts = vgc_stack_new(heap,NULL,consts_size,area_static);
  if(!consts){
    uabort("vvm:create consts error!");
  }

  symtb = uhash_table_new(VVM_SYMTB_SIZE);
  if(!symtb){
    uabort("vvm:create symtb error!");
  }

  unew(vm,
       sizeof(vvm),
       uabort("vvm:create vvm error!"););
  
  vm->heap    = heap;
  vm->symtb   = symtb;
  vm->consts  = consts;

  ctx = vcontext_new(vm,stack_size);
  if(!ctx){
    uabort("vvm:create context error!");
  }

  vm->context = ctx;
  return vm;
}

void* vvm_symtb_key_put(void* key){
  vsymbol* sym = key;
  vsymbol* new_sym;
  new_sym = vsymbol_new(sym->name,sym->index);
  return new_sym;
}

int vvm_symtb_key_comp(void* k1,void* k2){
  vsymbol* sym1 = k1;
  vsymbol* sym2 = k2;
  return sym1->name - sym2->name;
}

int vvm_obj_put(vvm* vm,char* name,vslot* obj){
  vsymbol sym;
  vsymbol* new_sym;
  uhash_table* symtb = vm->symtb;
  vgc_stack*   consts = vm->consts;
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
  new_sym = uhash_table_put(symtb,
			    str->hash_code % VVM_SYMTB_SIZE,
			    &sym,
			    vvm_symtb_key_put,
			    vvm_symtb_key_comp);
  if(!new_sym){
    return -1;
  }else{
    return index;
  }
}

void* vvm_symtb_key_get(void* key){
  return key;
}

vslot* vvm_obj_get(vvm* vm,char* name){
  int index = vvm_obj_get_index(vm,name);
  if(vgc_obj_ref_check(vm->consts,index)){
    return &vm->consts->objs[index];
  }else{
    return NULL;
  }
}

int vvm_obj_get_index(vvm* vm,char* name){
  ustring* str;
  uhash_table* symtb = vm->symtb;
  vsymbol sym;
  vsymbol* get_sym;
  str = vstrtb_put(name,-1);
  if(!str){
    return -1;
  }
  sym = vsymbol_init(str,-1);
  get_sym = uhash_table_get(symtb,
			    str->hash_code % VVM_SYMTB_SIZE,
			    &sym,
			    vvm_symtb_key_get,
			    vvm_symtb_key_comp);
  if(!get_sym){
    return -1;
  }
  return get_sym->index;
}

void bc_top(vcontext* ctx,usize_t top){
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  if(!vgc_obj_ref_check(stack,top))
    uabort("vvm:stack top error!");
  stack->top=top;
}

void bc_push(vcontext* ctx,
	     vslot     slot){
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  if(vgc_obj_is_full(stack))
    uabort("stack overflow!");
  vgc_stack_push(stack,slot);
}

vslot bc_pop(vcontext* ctx){
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  if(vgc_obj_is_empty(stack))
    uabort("vvm:stack empty!");
  return vgc_stack_pop(stack);
}

vslot bc_locals(vcontext* ctx,
		usize_t   index){
  vgc_call* curr_call = vslot_ref_get(ctx->curr_call);
  vgc_stack* locals   = vslot_ref_get(curr_call->locals);
  if(!vgc_obj_ref_check(locals,index))
    uabort("vvm:local varable error!");
  return locals->objs[index];
}

void bc_store(vcontext* ctx,
	      usize_t index){
  vslot slot          = bc_pop(ctx);
  vgc_call* curr_call = vslot_ref_get(ctx->curr_call);
  vgc_stack* locals   = vslot_ref_get(curr_call->locals);
  if(!vgc_obj_ref_check(locals,index))
    uabort("vvm:local varable error!");
  locals->objs[index] = slot;
}

void bc_ref(vcontext* ctx,
	    usize_t index){
  vslot slot   = bc_pop(ctx);
  vgc_obj* obj = vslot_ref_get(slot);
  if(!obj)
    uabort("vvm:refrence null object!");
  if(!vgc_obj_ref_check(obj,index))
    uabort("vvm:ref obj error!");
  bc_push(ctx,vgc_obj_ref_list(obj)[index]);
}

void bc_set(vcontext* ctx,
	    usize_t   index){
  vslot slot_obj = bc_pop(ctx);
  vslot slot_val = bc_pop(ctx);
  vgc_obj* obj   = vslot_ref_get(slot_obj);
  if(!obj)
    uabort("vvm:set null object!");
  if(!vgc_obj_ref_check(obj,index))
    uabort("vvm:set obj error!");
  vgc_obj_ref_list(obj)[index]=slot_val;
}

void bc_add(vcontext* ctx){
  vslot slot1 = bc_pop(ctx);
  vslot slot2 = bc_pop(ctx);
  vslot num;
  if(!vslot_is_num(slot1) ||
     !vslot_is_num(slot2)){
    uabort("vvm:add not a number!");
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
    uabort("vvm:eq type error!");
  }
  bc_push(ctx,bool);
}

void bc_jmp(vcontext* ctx,
	    usize_t   offset){
  vgc_call* curr_call = vslot_ref_get(ctx->curr_call);
  vgc_subr* subr      = vslot_ref_get(curr_call->subr);
  vgc_str* bc         = vslot_ref_get(subr->bc);
  if(!vgc_str_bound_check(bc,offset))
    uabort("vvm:jmp error!");
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
  vgc_call* curr_call = vslot_ref_get(ctx->curr_call);
  vgc_subr* subr      = vslot_ref_get(curr_call->subr);
  vgc_stack* consts   = vslot_ref_get(subr->consts);
  if(!vgc_obj_ref_check(consts,index))
    uabort("vvm:constant error!");
  return consts->objs[index];
}

vslot* bc_locals_new(vcontext* ctx,
		     usize_t para_len,
		     usize_t local_len){
  usize_t i = 0;
  usize_t locals_len = para_len + local_len;
  vgc_stack* locals = vgc_stack_new(ctx->vm->heap,
				    ctx->cache,
				    locals_len,
				    area_active);
  if(!locals){
    uabort("out of memory!");
  }
  while(i < para_len){
    vslot para = bc_pop(ctx);
    vgc_stack_push(locals,para);
    i++;
  }
  vgc_obj_flip(locals);
  return vgc_stack_top_ref(ctx->cache);
}

void bc_call(vcontext* ctx){
  vgc_heap* heap   = ctx->vm->heap;
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  vslot* curr_call = &ctx->curr_call;
  vslot* slotp     = vcontext_cache(ctx,bc_pop(ctx));
  vgc_obj* obj;
  if(!vslot_is_ref(*slotp)){
    uabort("bc_call:not refrence can not execute!");
  }
  obj = vslot_ref_get(*slotp);
  if(VGCTYPEOF(obj,gc_subr)){
    vgc_subr* subr = (vgc_subr*)obj;
    vslot* locals  =
      bc_locals_new(ctx,subr->para_len,subr->local_len);
    vslot* call    =
      vgc_call_new(heap,
		   ctx->cache,
		   stack->top,
		   slotp,
		   NULL,
		   locals,
		   curr_call);
    if(!call)
      uabort("bc_call:subr out of memory!");
    bc_push(ctx,*call);
    ctx->curr_call = *call;
    vcontext_cache_clean(ctx);
  } else if(VGCTYPEOF(obj,gc_cfun)){
    vgc_cfun* cfun = (vgc_cfun*)obj;
    vslot* locals  =
      bc_locals_new(ctx,cfun->para_len,cfun->local_len);
    vslot* call    =
      vgc_call_new(heap,
		   ctx->cache,
		   stack->top,
		   NULL,
		   slotp,
		   locals,
		   curr_call);
    if(!call)
      uabort("bc_call:cfun out of memory!");
    bc_push(ctx,*call);
    ctx->curr_call = *call;
    vcontext_cache_clean(ctx);
    cfun->entry(ctx);
    bc_top(ctx,((vgc_call*)vslot_ref_get(*call))->base);
    bc_push(ctx,ctx->retval);
  }else{
    uabort("bc_call:can not execute!");
  }
}

void bc_return(vcontext* ctx){
  vgc_call* curr_call = vslot_ref_get(ctx->curr_call);
  vslot slot_val      = bc_pop(ctx);
  bc_top(ctx,curr_call->base);
  bc_push(ctx,slot_val);
  ctx->curr_call = curr_call->caller;
}

void bc_return_void(vcontext* ctx){
  vgc_call* curr_call = vslot_ref_get(ctx->curr_call);
  bc_top(ctx,curr_call->base);
  bc_push(ctx,VSLOT_NULL);
  ctx->curr_call = curr_call->caller;
}

#define CHECK_CURR_CALL						 \
  if(vslot_is_null(ctx->curr_call) ||				 \
     vgc_call_is_cfun((vgc_call*)vslot_ref_get(ctx->curr_call))) \
    goto context_exit

#define FETCH (*(((vgc_call*)vslot_ref_get(ctx->curr_call))->pc)++)

#define NEXT (op=FETCH)

#define NEXT2 do{op=FETCH;op+=FETCH<<8;}while(0)

void vcontext_execute(vcontext* ctx,
		      vslot*    slotp_entry){
  bc_push(ctx,*slotp_entry);
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