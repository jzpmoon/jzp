#include "vm.h"
#include "ualloc.h"
#include "uerror.h"
#include <stddef.h>

vinst*
vinst_new(usize_t code,usize_t operand){
  vinst* inst;
  unew(inst,sizeof(vinst),uabort("vinst:new error!"););
  inst->code = code;
  inst->operand = operand;
  return inst;
}

usize_t
_vinst_length(ulist* insts){
  ulist* header=insts;
  ulist* node=insts;
  usize_t length=0;
  do{
    vinst* inst=node->value;
    switch(inst->code){
      #define DF(code,name,value,len) \
      case (code):length+=len;break;
      VBYTE_CODE
      #undef DF
    }
  } while((node=node->next)!=header);
  return length;
}

usize_t
_vinst_offset_length(ulist* insts,usize_t offset){
  ulist* node = insts;
  usize_t length = 0;
  usize_t count = offset >= 0 ? offset : -offset;
  usize_t i;
  for(i = 0;i < count;i++){
    vinst* inst=node->value;
    switch(inst->code){
      #define DF(code,name,value,len)			\
      case (code):length+=len;break;
      VBYTE_CODE
      #undef DF
    }
    if(offset > 0){
      node = node->next;
    }else{
      node = node->prev;
    }
  }
  return offset >= 0 ? length : -length;
}

vgc_str*
vinst_to_str(vgc_heap* heap,ulist* insts){
  ulist* header = insts;
  ulist* node = insts;
  int total = 0;
  usize_t length = _vinst_length(insts);
  vgc_str* str = vgc_str_new(heap,length);
  usize_t count = 0;
  if(!str) return NULL;
  do{
    vinst* inst=node->value;
    switch(inst->code){
#define DF(ocode,name,value,len)					\
      case (ocode):{							\
	int i = 0;							\
	usize_t operand;						\
	if(ocode == Bjmp || ocode == Bjmpi){				\
	  operand = total + _vinst_offset_length(node,inst->operand);	\
	}else{								\
	  operand = inst->operand;					\
	}								\
	str->u.b[count++] = inst->code;					\
	while(i<len-1&&i<sizeof(usize_t)){				\
	  str->u.b[count++] = operand>>(8*i);				\
	  i++;								\
	}								\
	total += len;							\
	break;								\
      }						
      VBYTE_CODE				
#undef DF
	}
  }while((node = node->next) != header);
  vgc_str_log(str);
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
    ctx->heap=heap;
    ctx->stack=stack;
    ctx->curr_call=NULL;
    vgc_obj_flip((vgc_obj*)ctx);
  }
  return ctx;
}

void bc_top(vcontext* ctx,usize_t top){
  vgc_stack* stack=ctx->stack;
  if(!vgc_obj_ref_check(stack,top))
    uabort("vm:stack top error!");
  stack->top=top;
}

void bc_push(vcontext* ctx,
	     vgc_obj* obj){
  vgc_stack* stack=ctx->stack;
  if(vgc_obj_is_full(stack))
    uabort("stack overflow!");
  vgc_stack_push(stack,obj);
}

vgc_obj* bc_pop(vcontext* ctx){
  vgc_stack* stack=ctx->stack;
  if(vgc_obj_is_empty(stack))
    uabort("vm:stack empty!");
  return vgc_stack_pop(stack);
}

vgc_obj*
bc_locals(vcontext* ctx,
	  usize_t index){
  vgc_stack* locals=
    ctx->curr_call->locals;
  if(!vgc_obj_ref_check(locals,index))
    uabort("vm:local varable error!");
  return locals->objs[index];
}

void bc_store(vcontext* ctx,
	      usize_t index){
  vgc_obj* obj=bc_pop(ctx);
  vgc_stack* locals=
    ctx->curr_call->locals;
  if(!vgc_obj_ref_check(locals,index))
    uabort("vm:local varable error!");
  locals->objs[index]=obj;
}

vgc_obj*
bc_pop_type(vcontext* ctx,int type){
  vgc_obj* obj=bc_pop(ctx);
  if(!VGCTYPEOF(obj,type))
    uabort("vm:pop type error!");
  return obj;
}

#define bc_pop_num(ctx)	    \
  (vgc_num*)bc_pop_type(ctx,gc_num)

#define bc_pop_bool(ctx)    \
  bc_pop_num(ctx)

void bc_ref(vcontext* ctx,
	    usize_t index){
  vgc_obj* obj=bc_pop(ctx);
  if(!obj)
    uabort("vm:refrence null object!");
  if(!vgc_obj_ref_check(obj,index))
    uabort("vm:ref obj error!");
  bc_push(ctx,vgc_obj_ref_list(obj)[index]);
}

void bc_set(vcontext* ctx,
	    usize_t index){
  vgc_obj* obj=bc_pop(ctx);
  vgc_obj* value=bc_pop(ctx);
  if(!obj)
    uabort("vm:set null object!");
  if(!vgc_obj_ref_check(obj,index))
    uabort("vm:set obj error!");
  vgc_obj_ref_list(obj)[index]=value;
}

void bc_add(vcontext* ctx){
  vgc_num* num1=bc_pop_num(ctx);
  vgc_num* num2=bc_pop_num(ctx);
  vgc_num* num=
    vgc_num_add(ctx->heap,num1,num2);
  if(!num)
    uabort("vm:out of memory!");
  bc_push(ctx,(vgc_obj*)num);
}

void bc_eq(vcontext* ctx){
  vgc_num* num1=bc_pop_num(ctx);
  vgc_num* num2=bc_pop_num(ctx);
  vgc_bool* bool=
    vgc_num_eq(ctx->heap,num1,num2);
  if(!bool)
    uabort("vm:out of memory!");
  bc_push(ctx,(vgc_obj*)bool);
}

void bc_jmp(vcontext* ctx,
	    usize_t offset){
  vgc_call* curr_call=ctx->curr_call;
  vgc_str* bc=curr_call->subr->bc;
  if(!vgc_str_bound_check(bc,offset))
    uabort("vm:jmp error!");
  curr_call->pc=bc->u.b+offset;
}

void bc_jmpi(vcontext* ctx,
	     usize_t offset){
  vgc_bool* bool=bc_pop_bool(ctx);
  if(VTRUEP(bool))
    bc_jmp(ctx,offset);
}

vgc_obj* bc_constant(vcontext* ctx,
		     usize_t index){
  vgc_stack* consts=
    ctx->curr_call->subr->consts;
  if(!vgc_obj_ref_check(consts,index))
    uabort("vm:constant error!");
  return consts->objs[index];
}

vgc_stack* bc_locals_new(vcontext* ctx,
			 vgc_subr* subr){
  usize_t locals_len=
    subr->para_len+subr->local_len;
  usize_t i=0;
  vgc_stack* locals=
    vgc_stack_new(ctx->heap,locals_len);
  if(!locals)
    uabort("out of memory!");
  while(i<subr->para_len){
    vgc_obj* para=bc_pop(ctx);
    vgc_stack_push(locals,para);
    i++;
  }
  vgc_obj_flip(locals);
  return locals;
}

void bc_call(vcontext* ctx){
  vgc_heap* heap=ctx->heap;
  vgc_stack* stack=ctx->stack;
  vgc_call* curr_call=ctx->curr_call;
  vgc_obj* obj=bc_pop(ctx);
  if(VGCTYPEOF(obj,gc_subr)){
    vgc_subr* subr=(vgc_subr*)obj;
    vgc_stack* locals=
      bc_locals_new(ctx,subr);
    vgc_call* call=
      vgc_call_new(heap,
		   stack->top,
		   subr,
		   locals,
		   curr_call);
    if(!call)
      uabort("out of memory!");
    bc_push(ctx,(vgc_obj*)call);
    ctx->curr_call=call;
  }else{
    uabort("cant't execute!");
  }
}

void bc_return(vcontext* ctx){
  vgc_call* curr_call=ctx->curr_call;
  vgc_obj* value=bc_pop(ctx);
  bc_top(ctx,curr_call->base);
  bc_push(ctx,value);
  ctx->curr_call=curr_call->caller;
}

void bc_return_void(vcontext* ctx){
  vgc_call* curr_call=ctx->curr_call;
  bc_top(ctx,curr_call->base);
  bc_push(ctx,NULL);
  ctx->curr_call=curr_call->caller;
}

#define CHECK_CURR_CALL \
  if(!ctx->curr_call) goto context_exit

#define NEXT (*(ctx->curr_call->pc)++)

/*#define NEXT2 ((NEXT)+(NEXT<<8))*/

#define NEXT2 _NEXT2(ctx)

usize_t _NEXT2 (vcontext * ctx) {
  usize_t op = NEXT;
  op = op + (NEXT<<8);
  return op;
}

void vcontext_execute(vcontext* ctx,
		      vgc_subr* subr){
  bc_push(ctx,(vgc_obj*)subr);
  bc_call(ctx);
  while(1){
    int op;
    CHECK_CURR_CALL;
    op=NEXT;
    switch(op){
    case Bpush:
      op=NEXT;
      ulog1("Bpush %d",op);
      bc_push(ctx,bc_constant(ctx,op));
      break;
    case Bpop:
      ulog("Bpop");
      bc_pop(ctx);
      break;
    case Bload:
      op=NEXT;
      ulog1("Bload %d",op);
      bc_push(ctx,bc_locals(ctx,op));
      break;
    case Bstore:
      op=NEXT;
      ulog1("Bstore %d",op);
      bc_store(ctx,op);
      break;
    case Bref:
      op=NEXT2;
      ulog1("Bref %d",op);
      bc_ref(ctx,op);
      break;
    case Bset:
      op=NEXT2;
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
      op=NEXT2;
      ulog1("Bjmpi %d",op);
      bc_jmpi(ctx,op);
      break;
    case Bjmp:
      op=NEXT2;
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
