#include <string.h>
#include "ualloc.h"
#include "uerror.h"
#include "vgc_obj.h"


int vgc_array_new(vgc_heap* heap,
		  usize_t len,
		  int area_type){
  usize_t size = TYPE_SIZE_OF(vgc_array,vslot,len);
  vgc_array* array;
  vslot* slot_list;
  int i = 0;
  if(vgc_heap_data_new
     (heap,size,len,vgc_obj_type_array,area_type)){
    return -1;
  }
  vgc_pop_obj(heap,array,vgc_array);
  slot_list = vgc_obj_slot_list(array);
  while(i < len){
    vslot_null_set(slot_list[i]);
    i++;
  }
  vgc_push_obj(heap,array);
  return 0;
}

int vgc_string_new(vgc_heap* heap,
		   usize_t len,
		   int area_type){
  usize_t size = TYPE_SIZE_OF(vgc_string,char,len);
  vgc_string* string;
  if(vgc_heap_data_new(heap,size,len,vgc_obj_type_string,area_type)){
    return -1;
  }
  vgc_pop_obj(heap,string,vgc_string);
  string->len = len;
  vgc_push_obj(heap,string);
  return 0;
}

void vgc_string_log(vgc_heap* heap){
  int i = 0;
  vgc_string* string;
  vgc_pop_obj(heap,string,vgc_string);
  while(i < string->len){
    ulog1("string:%d",string->u.b[i]);
    i++;
  }
  vgc_push_obj(heap,string);
}

int vgc_cfun_new(vgc_heap* heap,
		 vcfun_ft entry,
		 int area_type){
  vgc_cfun* cfun;
  if(vgc_heap_obj_new(heap,vgc_cfun,vgc_obj_type_cfun,area_type)){
    return -1;
  }
  vgc_pop_obj(heap,cfun,vgc_cfun);
  cfun->entry = entry;
  vgc_push_obj(heap,cfun);
  return 0;
}

int vgc_subr_new(vgc_heap* heap,
		 usize_t params_count,
		 usize_t locals_count,
		 int area_type){
  vgc_subr* subr;
  if(vgc_heap_obj_new(heap,vgc_subr,vgc_obj_type_subr,area_type)){
    return -1;
  }
  vgc_pop_obj(heap,subr,vgc_subr);
  subr->params_count = params_count;
  subr->locals_count = locals_count;
  vgc_obj_slot_set(heap,subr,consts);
  vgc_obj_slot_set(heap,subr,bytecode);
  vgc_push_obj(heap,subr);
  return 0;
}

int vgc_call_new(vgc_heap* heap,
		 int call_type,
		 usize_t base){
  vgc_call* call;
  vgc_subr* subr;
  vgc_string* bytecode;
  if(vgc_heap_obj_new
     (heap,vgc_call,vgc_obj_type_call,vgc_heap_area_active)){
    return -1;
  }
  vgc_pop_obj(heap,call,vgc_call);
  call->call_type = call_type;
  call->base = base;
  if(call_type == vgc_call_type_cfun){
    vgc_obj_slot_set(heap,call,cfun);
  }else{
    vgc_pop_obj(heap,subr,vgc_subr);
    vgc_obj_slot_get(heap,subr,bytecode);
    vgc_pop_obj(heap,bytecode,vgc_string);
    call->pc = bytecode->u.b;
    vgc_push_obj(heap,subr);
    vgc_obj_slot_set(heap,call,subr);
  }
  vgc_obj_slot_set(heap,call,caller);
  vgc_push_obj(heap,call);
  return 0;
}

vslot vslot_num_add(vslot slot1,vslot slot2){
  double num1 = vslot_num_get(slot1);
  double num2 = vslot_num_get(slot2);
  double sum = num1 + num2;
  vslot slot3;
  vslot_num_set(slot3,sum);
  return slot3;
}

vslot vslot_num_eq(vslot slot1,vslot slot2){
  double num1 = vslot_num_get(slot1);
  double num2 = vslot_num_get(slot2);
  vslot bool;
  if(num1 == num2){
    vslot_bool_set(bool,1);
  }else{
    vslot_bool_set(bool,0);
  }
  return bool;
}

vslot vslot_ref_eq(vslot slot1,vslot slot2){
  vgc_obj* ref1 = vslot_ref_get(slot1,vgc_obj);
  vgc_obj* ref2 = vslot_ref_get(slot2,vgc_obj);
  vslot bool;
  if(ref1 == ref2){
    vslot_bool_set(bool,1);
  }else{
    vslot_bool_set(bool,0);
  }
  return bool;
}

int vgc_extend_new(vgc_heap* heap,
		   vgc_objex_t* oet){
  vgc_extend* extend;
  if(vgc_heap_obj_new
     (heap,vgc_extend,vgc_obj_type_extend,vgc_heap_area_active)){
    return -1;
  }
  vgc_pop_obj(heap,extend,vgc_extend);
  extend->oet = oet;
  vgc_push_obj(heap,extend);
  return 0;
}
