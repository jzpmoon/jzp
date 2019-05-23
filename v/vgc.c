#include <string.h>
#include "vgc_obj.h"
#include "ustack.h"
#include "ualloc.h"
#include "uerror.h"

#define _IS_MARK(O) ((O)->mark.m)
#define _MARK(O) ((O)->mark.m=1)
#define _UNMARK(O) ((O)->mark.m=0)
#define _IS_ADDR(O) ((O)->mark.a)
#define _MARK_ADDR(O) ((O)->mark.a=1)
#define _UNADDR(O) ((O)->mark.a=0)
#define _INITMARK(O,T) \
  (O)->mark.m=0;       \
  (O)->mark.a=0;       \
  (O)->mark.t=(T)

#define _NEXT_OBJ(O,S)		 \
  ((vgc_obj*)((void*)(O)+        \
	     ALIGN_BOUNDARY(S)))

#define _LAST_OBJ(H) \
  ((void*)(H)->mem+(H)->size);

void mark_obj(vgc_obj* root,
	      ustack* stack){
  if(ustack_push_obj(stack,root))
    uabort("vgc_heap:stack overflow!");
  while(1){
    vgc_obj** ref_list;
    vgc_obj* tmp_obj;
    usize_t i;
    if(ustack_pop_obj(stack,tmp_obj))
      return;
    if(!tmp_obj||_IS_MARK(tmp_obj))
      continue;
    _MARK(tmp_obj);
    ref_list=vgc_obj_ref_list(tmp_obj);
    for(i=0;i<tmp_obj->top;i++){
      if(ustack_push_obj(stack,ref_list[i]))
	uabort("vgc_heap:stack overflow!");
    }
  }
}

void cal_addr(vgc_obj* bg_obj,
	      vgc_obj* ed_obj){
  vgc_obj* tmp_obj=bg_obj;
  vgc_obj* new_addr=bg_obj;
  while(tmp_obj<ed_obj){
    if(_IS_MARK(tmp_obj)){
      _MARK_ADDR(tmp_obj);
      tmp_obj->addr=new_addr;
      new_addr=
	_NEXT_OBJ(new_addr,
		  tmp_obj->size);
    }
    tmp_obj=_NEXT_OBJ(tmp_obj,
		      tmp_obj->size);
  }
}

vgc_obj* update_addr(vgc_obj* root,
		     ustack* stack){
  if(ustack_push_obj(stack,root))
    uabort("vgc_heap:stack overflow!");
  while(1){
    vgc_obj** ref_list;
    vgc_obj* tmp_obj;
    usize_t i;
    if(ustack_pop_obj(stack,tmp_obj)){
      if(root)
	return root->addr;
      else
	return NULL;
    }
    if(!tmp_obj||!_IS_ADDR(tmp_obj))
      continue;
    _UNADDR(tmp_obj);
    ref_list=vgc_obj_ref_list(tmp_obj);
    for(i=0;i<tmp_obj->top;i++){
      vgc_obj* ref_obj=ref_list[i];
      if(ref_obj)
	ref_list[i]=ref_obj->addr;
      if(ustack_push_obj(stack,ref_obj))
	uabort("vgc_heap:stack overflow!");
    }
  }
}

vgc_obj* move_obj(vgc_obj* bg_obj,
		  vgc_obj* ed_obj){
  vgc_obj* tmp_obj=bg_obj;
  vgc_obj* new_addr=bg_obj;
  while(tmp_obj<ed_obj){
    vgc_obj* next_obj=
      _NEXT_OBJ(tmp_obj,tmp_obj->size);
    if(_IS_MARK(tmp_obj)){
      _UNMARK(tmp_obj);
      new_addr=_NEXT_OBJ(new_addr,
			tmp_obj->size);
      memcpy(tmp_obj->addr,
	     tmp_obj,
	     tmp_obj->size);
    }
    tmp_obj=next_obj;
  }
  return new_addr;
}

void vgc_collect(vgc_heap* heap){
  ulog("vgc_heap:Collect start");
  mark_obj(heap->rootset,
	   &heap->stack);
  ulog("vgc_heap:Mark end");
  cal_addr(heap->mem,
	   heap->end);
  ulog("vgc_heap:Cal Address end");
  heap->rootset=
    update_addr(heap->rootset,
		&heap->stack);
  ulog("vgc_heap:Update Address end");
  heap->end=move_obj(heap->mem,
		     heap->end);
  ulog("vgc_heap:Collect end");
}

vgc_obj* vgc_try_new(vgc_heap* heap,
		     usize_t size,
		     usize_t len,
		     int type){
  vgc_obj* new_obj=NULL;
  vgc_obj* next_obj=
    _NEXT_OBJ(heap->end,size);
  vgc_obj* last_obj=_LAST_OBJ(heap);
  if(next_obj<=last_obj){
    new_obj=heap->end;
    new_obj->addr=NULL;
    new_obj->size=size;
    new_obj->len=len;
    new_obj->top=0;
    _INITMARK(new_obj,type);
    heap->end=next_obj;
  }
  return new_obj;
}

vgc_obj*
_vgc_heap_obj_new(vgc_heap* heap,
		  usize_t size,
		  usize_t len,
		  int type){
  vgc_obj* new_obj=
    vgc_try_new(heap,size,len,type);
  if(!new_obj){
    vgc_collect(heap);
    new_obj=
      vgc_try_new(heap,size,len,type);
  }
  return new_obj;
}

vgc_stack*
vgc_stack_new(vgc_heap* heap,
	      usize_t len){
  usize_t size=
    sizeof(vgc_stack)+
    sizeof(vgc_obj*)*(len-1);
  vgc_stack* stack=(vgc_stack*)
    _vgc_heap_obj_new(heap,
		      size,
		      len,
		      gc_stack);
  return stack;
}

void vgc_stack_push(vgc_stack* stack,
		    vgc_obj* obj){
  if(!vgc_obj_is_full(stack)){
    vgc_obj** objs=stack->objs;
    objs[stack->top++]=obj;
  }
}

vgc_obj*
vgc_stack_pop(vgc_stack* stack){
  if(!vgc_obj_is_empty(stack)){
    vgc_obj** objs=stack->objs;
    return objs[--stack->top];
  }
  return NULL;
}

vgc_stack*
vgc_stack_expand(vgc_heap* heap,
		 vgc_stack* stack){
  usize_t new_len=stack->len*2;
  vgc_stack* new_stack=
    vgc_stack_new(heap,new_len);
  if(new_stack){
    memcpy(new_stack,stack,stack->size);
    new_stack->len=new_len;
  }
  return new_stack;
}

int vgc_heap_obj_reg(vgc_heap* heap,
		      vgc_obj* obj){
  vgc_stack* stack=
    (vgc_stack*)heap->rootset;
  if(!stack){
    stack=vgc_stack_new(heap,1);
    if(!stack) return -1;
  }
  if(vgc_obj_is_full(stack)){
    stack=vgc_stack_expand(heap,stack);
    if(!stack) return -1;
  }
  vgc_stack_push(stack,obj);
  heap->rootset=(vgc_obj*)stack;
  return 0;
}

vgc_str*
vgc_str_new(vgc_heap* heap,
	    usize_t str_len){
  usize_t size=
    sizeof(vgc_str)+
    sizeof(char)*(str_len-1);
  vgc_str* str=(vgc_str*)
    _vgc_heap_obj_new(heap,
		      size,
		      0,
		      gc_str);
  if(str){
    str->str_len=str_len;
  }
  return str;
}

vgc_subr*
vgc_subr_new(vgc_heap* heap,
	     usize_t para_len,
	     usize_t local_len,
	     vgc_str* bc,
	     vgc_stack* consts){
  vgc_subr* subr=(vgc_subr*)
    vgc_heap_obj_new(heap,
		     vgc_subr,
		     2,
		     gc_subr);
  if(subr){
    subr->para_len=para_len;
    subr->local_len=local_len;
    subr->bc=bc;
    subr->consts=consts;
    vgc_obj_flip(subr);
  }
  return subr;
}

vgc_call*
vgc_call_new(vgc_heap* heap,
	     usize_t base,
	     vgc_subr* subr,
	     vgc_stack* locals,
	     vgc_call* caller){
  vgc_call* call=(vgc_call*)
    vgc_heap_obj_new(heap,
		     vgc_call,
		     3,
		     gc_call);
  if(call){
    call->pc=subr->bc->u.b;
    call->base=base;
    call->subr=subr;
    call->locals=locals;
    call->caller=caller;
    vgc_obj_flip(call);
  }
  return call;
}

vgc_num*
vgc_num_new(vgc_heap* heap,
	    float value){
  vgc_num* num=(vgc_num*)
    vgc_heap_obj_new(heap,
		     vgc_num,
		     0,
		     gc_num);
  if(num){
    num->value=value;
  }
  return num;
}

vgc_num*
vgc_num_add(vgc_heap* heap,
	    vgc_num* num1,
	    vgc_num* num2){
  float value=num1->value+num2->value;
  vgc_num* num=vgc_num_new(heap,value);
  return num;
}

vgc_bool*
vgc_num_eq(vgc_heap* heap,
	   vgc_num* num1,
	   vgc_num* num2){
  if(num1->value==num2->value){
    return vgc_true_new(heap);
  }else{
    return vgc_false_new(heap);
  }
}
