#include "ualloc.h"
#include "uerror.h"
#include "vgc_obj.h"

vgc_stack* vgc_stack_new(vgc_heap* heap,
			 usize_t   len) {
  usize_t size = TYPE_SIZE_OF(vgc_stack,vslot,len);
  vgc_stack* stack = (vgc_stack*)
    _vgc_heap_obj_new(heap,size,len,gc_stack,area_active);
  return stack;
}

void vgc_stack_push(vgc_stack* stack,
		    vslot      slot) {
  if(!vgc_obj_is_full(stack)) {
    vslot* objs = stack->objs;
    objs[stack->top++] = slot;
  }
}

vslot vgc_stack_pop(vgc_stack* stack){
  if(!vgc_obj_is_empty(stack)){
    vslot* objs = stack->objs;
    return objs[--stack->top];
  }
  return VSLOT_NULL;
}

vgc_stack* vgc_stack_expand(vgc_heap*  heap,
			    vgc_stack* stack) {
  usize_t new_len = stack->len > 0 ? stack->len*2 : 1;
  vgc_stack* new_stack = vgc_stack_new(heap,new_len);
  if(new_stack){
    int i;
    for(i = 0;i < stack->len;i++){
      vgc_stack_push(new_stack,stack->objs[i]);
    }
  }
  return new_stack;
}

vgc_str* vgc_str_new(vgc_heap* heap,
		     usize_t   str_len) {
  usize_t size = TYPE_SIZE_OF(vgc_str,char,str_len);
  vgc_str* str=(vgc_str*)
    _vgc_heap_obj_new(heap,size,0,gc_str,area_active);
  if(str){
    str->str_len = str_len;
  }
  return str;
}

vgc_str* vgc_str_newc(vgc_heap*  heap,
		      char*      charp,
		      usize_t    str_len){
  vgc_str* str = vgc_str_new(heap,str_len);
  if(str){
    memcpy(str->u.b,charp,str_len);
  }
  return str;
}

void vgc_str_log(vgc_str* str) {
  usize_t i = 0;
  while (i < str->str_len) {
    ulog1("vgc_str:%d",str->u.b[i++]);
  }
}

vgc_subr* vgc_subr_new(vgc_heap*  heap,
		       usize_t    para_len,
		       usize_t    local_len,
		       vgc_str*   bc,
		       vgc_stack* consts) {
  vgc_subr* subr = (vgc_subr*)
    vgc_heap_obj_new(heap,vgc_subr,2,gc_subr,area_active);
  if(subr){
    subr->para_len  = para_len;
    subr->local_len = local_len;
    vslot_ref_set(subr->bc,bc);
    vslot_ref_set(subr->consts,consts);
    vgc_obj_flip(subr);
  }
  return subr;
}

vgc_call* vgc_call_new(vgc_heap*  heap,
		       usize_t    base,
		       vgc_subr*  subr,
		       vgc_stack* locals,
		       vgc_call*  caller) {
  vgc_call* call = (vgc_call*)
    vgc_heap_obj_new(heap,vgc_call,3,gc_call,area_active);
  if(call){
    vgc_str* str = (vgc_str*)vslot_ref_get(subr->bc);
    call->pc     = str->u.b;
    call->base   = base;
    vslot_ref_set(call->subr,subr);
    vslot_ref_set(call->locals,locals);
    vslot_ref_set(call->caller,caller);
    vgc_obj_flip(call);
  }
  return call;
}

vslot vslot_num_add(vslot num1,
		    vslot num2){
  double _num1;
  double _num2;
  double sum;
  vslot  slot;
  _num1 = vslot_num_get(num1);
  _num2 = vslot_num_get(num2);
  sum   = _num1 + _num2;
  vslot_num_set(slot,sum);
  return slot;
}

vslot vslot_num_eq(vslot num1,
		   vslot num2){
  double _num1;
  double _num2;
  vslot  slot;
  _num1 = vslot_num_get(num1);
  _num2 = vslot_num_get(num2);
  if(_num1 == _num2){
    vslot_bool_set(slot,VTRUE);
  }else{
    vslot_bool_set(slot,VFALSE);
  }
  return slot;
}

vslot vslot_ref_eq(vslot ref1,
		   vslot ref2){
  vgc_obj* _ref1;
  vgc_obj* _ref2;
  vslot  slot;
  _ref1 = vslot_ref_get(ref1);
  _ref2 = vslot_ref_get(ref2);
  if(_ref1 == _ref2){
    vslot_bool_set(slot,VTRUE);
  }else{
    vslot_bool_set(slot,VFALSE);
  }
  return slot;
}

void vgc_obj_log(vgc_obj* obj){
  ulog1("type:%d",obj->mark.t);
  ulog1("size:%d",obj->size);
  ulog1("len :%d",obj->len);
  ulog1("top :%d",obj->top);
}

vgc_objex* _vgc_objex_new(vgc_heap*   heap,
			usize_t     objex_size,
			usize_t     slot_len,
			vgc_objex_t objex_type,
			int         area_type){
  vgc_objex* objex = (vgc_objex*)
    _vgc_heap_obj_new(heap,
		      objex_size,
		      slot_len,
		      gc_extend,
		      area_type);
  if(objex){
    objex->oet = objex_type;
  }
  return objex;
}

#define VGCHEAP_STRTB_LEN 17
static ustring_table _strtb[VGCHEAP_STRTB_LEN];

#define VGCHEAP_TYTB_LEN 17
static uhash_table _tytb[VGCHEAP_TYTB_LEN];

static vgc_objex_t* vtytb_key_put(void* key){
  vgc_objex_t* type;
  ustring* str = (ustring*)key;
  unew(type,
       sizeof(vgc_objex_t),
       uabort("new vgc_objex_t error!"););
  type->type_name = str;
  return type;
}

static int vtytb_comp(void* k1,void* k2){
  return k1-k2;
}

ustring* vstrtb_put(char* charp,int len){
  ustring* str = ustring_table_put(_strtb,
				   VGCHEAP_STRTB_LEN,
				   charp,
				   len);
  return str;
}

vgc_objex_t* vgc_objex_init(char* charp){
  ustring* str = vstrtb_put(charp,-1);
  vgc_objex_t type;
  if(!str){
    type = uhash_table(_tytb,
		       str->hash_code % VGCHEAP_TYTB_LEN,
		       str,
		       vtytb_key_put,
		       vtytb_comp);
  }
  return type;
}
