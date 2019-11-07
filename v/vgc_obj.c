#include <string.h>
#include "ualloc.h"
#include "uerror.h"
#include "vgc_obj.h"

vgc_stack* vgc_stack_new(vgc_heap*  heap,
			 vgc_stack* stack,
			 usize_t    len,
			 int        area_type) {
  usize_t size = TYPE_SIZE_OF(vgc_stack,vslot,len);
  vgc_stack* new_stack = (vgc_stack*)
    _vgc_heap_obj_new(heap,stack,size,len,gc_stack,area_type);
  return new_stack;
}

int vgc_stack_push(vgc_stack* stack,
		   vslot      slot) {
  if(!vgc_obj_is_full(stack)) {
    vslot* objs = stack->objs;
    objs[stack->top] = slot;
    return stack->top++;
  }
  return -1;
}

vslot vgc_stack_pop(vgc_stack* stack){
  if(!vgc_obj_is_empty(stack)){
    vslot* objs = stack->objs;
    return objs[--stack->top];
  }
  return VSLOT_NULL;
}

vslot* vgc_stack_top_ref(vgc_stack* stack){
  if(stack->top > 0){
    return &stack->objs[stack->top - 1];
  }else{
    return NULL;
  }
}

vslot* vgc_str_new(vgc_heap*  heap,
		   vgc_stack* stack,
		   usize_t    str_len,
		   int        area_type) {
  usize_t size = TYPE_SIZE_OF(vgc_str,char,str_len);
  vgc_str* str = (vgc_str*)
    _vgc_heap_obj_new(heap,stack,size,0,gc_str,area_type);
  if(str){
    str->str_len = str_len;
    return vgc_stack_top_ref(stack);
  }
  return NULL;
}

vslot* vgc_str_newc(vgc_heap*  heap,
		    vgc_stack* stack,
		    char*      charp,
		    usize_t    str_len,
		    int        area_type){
  vslot* slotp = vgc_str_new(heap,stack,str_len + 1,area_type);
  if(slotp){
    vgc_str* str = vslot_ref_get(*slotp);
    memcpy(str->u.c,charp,str_len);
    str->u.c[str_len] = '\0';
  }
  return slotp;
}

vslot* vgc_str_new_by_buff(vgc_heap*  heap,
			   vgc_stack* stack,
			   ubuffer*   buff,
			   int        area_type){
  char*    begin;
  int      len;
  vslot*   slotp;
  ubuffer_ready_read(buff);			
  begin = buff->data;				
  len   = ubuffer_stock(buff);		
  ubuffer_ready_write(buff);			
  slotp   = vgc_str_newc(heap,stack,begin,len,area_type);
  return slotp;
}

void vgc_str_log(vgc_str* str) {
  usize_t i = 0;
  while (i < str->str_len) {
    ulog1("vgc_str:%d",str->u.b[i++]);
  }
}

vslot* vgc_subr_new(vgc_heap*  heap,
		    vgc_stack* stack,
		    usize_t    para_len,
		    usize_t    local_len,
		    vslot*     bc,
		    vslot*     consts){
  vgc_subr* subr =
    vgc_heap_obj_new(heap,stack,vgc_subr,2,gc_subr,area_active);
  if(subr){
    subr->para_len  = para_len;
    subr->local_len = local_len;
    subr->bc        = *bc;
    subr->consts    = *consts;
    vgc_obj_flip(subr);
    return vgc_stack_top_ref(stack);
  }
  return NULL;
}

vslot* vgc_cfun_new(vgc_heap*  heap,
		    vgc_stack* stack,
		    usize_t    para_len,
		    vcfun_t    entry,
		    int        area_type){
  vgc_cfun* cfun =
    vgc_heap_obj_new(heap,stack,vgc_cfun,0,gc_cfun,area_type);
  if(cfun){
    cfun->para_len = para_len;
    cfun->local_len = 0;
    cfun->entry = entry;
    return vgc_stack_top_ref(stack);
  }
  return NULL;
}

vslot* vgc_call_new(vgc_heap*  heap,
		    vgc_stack* stack,
		    usize_t    base,
		    vslot*     slotp_subr,
		    vslot*     slotp_cfun,
		    vslot*     slotp_locals,
		    vslot*     slotp_caller) {
  vgc_call* call =
    vgc_heap_obj_new(heap,stack,vgc_call,4,gc_call,area_active);
  if(call){
    if(slotp_subr){
      vgc_subr* subr = vslot_ref_get(*slotp_subr);
      vgc_str* str   = vslot_ref_get(subr->bc);
      call->pc       = str->u.b;
      call->base     = base;
      call->subr     = *slotp_subr;
      call->cfun     = VSLOT_NULL;
      call->locals   = *slotp_locals;
      call->caller   = *slotp_caller;
    }else if(slotp_cfun){
      call->pc     = NULL;
      call->base   = base;
      call->subr   = VSLOT_NULL;
      call->cfun   = *slotp_cfun;
      call->locals = *slotp_locals;
      call->caller = *slotp_caller;
    }else{
      return NULL;
    }
    vgc_obj_flip(call);
    return vgc_stack_top_ref(stack);
  }
  return NULL;
}

void vslot_log(vslot slot){
  ulog1("vslot type:%d",slot.t);
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

vslot* _vgc_objex_new(vgc_heap*    heap,
		      vgc_stack*   stack,
		      usize_t      objex_size,
		      usize_t      slot_len,
		      vgc_objex_t* objex_type,
		      int          area_type){
  vgc_objex* objex = (vgc_objex*)
    _vgc_heap_obj_new(heap,
		      stack,
		      objex_size,
		      slot_len,
		      gc_extend,
		      area_type);
  if(objex){
    objex->oet = objex_type;
    vgc_obj_flip(objex);
    return vgc_stack_top_ref(stack);
  }
  return NULL;
}

#define VGCHEAP_STRTB_LEN 17
static ustring_table _strtb[VGCHEAP_STRTB_LEN];

#define VGCHEAP_TYTB_LEN 17
static uhash_table _tytb[VGCHEAP_TYTB_LEN];

static void* vtytb_key_put(void* key){
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

ustring* vstrtb_put_by_buff(ubuffer* buff){
  char*    begin;
  int      len;
  ustring* str;
  ubuffer_ready_read(buff);
  begin = buff->data;
  len   = ubuffer_stock(buff);
  ubuffer_ready_write(buff);
  str   = vstrtb_put(begin,len);
  return str;
}

vgc_objex_t* vgc_objex_init(char* charp){
  ustring* str = vstrtb_put(charp,-1);
  vgc_objex_t* type = NULL;
  if(str){
    type = uhash_table_put(_tytb,
			   str->hash_code % VGCHEAP_TYTB_LEN,
			   str,
			   vtytb_key_put,
			   vtytb_comp);
  }else{
    uabort("vgc_objex_init error!");
  }
  return type;
}
