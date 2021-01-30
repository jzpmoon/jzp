#include <string.h>
#include "ualloc.h"
#include "uerror.h"
#include "vgc_obj.h"

vgc_array* vgc_array_new(vgc_heap* heap,
			 usize_t array_length,
			 int area_type){
  usize_t array_size = TYPE_SIZE_OF(vgc_array,vslot,array_length);
  vgc_array* array;
  vslot* slot_list;
  int i = 0;
  array = (vgc_array*)vgc_heap_data_new(heap,
					array_size,
					array_length,
					vgc_obj_type_array,
					area_type);
  if(array){
    array->top = 0;
    slot_list = vgc_obj_slot_list(array);
    while(i < array_length){
      vslot_null_set(slot_list[i]);
      i++;
    }
  }
  return array;
}

int vgc_array_push(vgc_array* array,vslot slot){
  if(array->top <= array->_len){
    array->objs[array->top] = slot;
    return array->top++;
  }else{
    return -1;
  }
}

vslot vgc_array_pop(vgc_array* array){
  if(array->top > 0){
    array->top--;
  }
  return array->objs[array->top];
}

void vgc_array_set(vgc_array* array,int idx,vslot slot){
  if (idx >= 0 && idx < array->_len) {
    array->objs[idx] = slot;
  }
}

vgc_string* vgc_string_new(vgc_heap* heap,
			   usize_t string_length,
			   int area_type){
  usize_t string_size = TYPE_SIZE_OF(vgc_string,char,string_length);
  vgc_string* string;
  string = (vgc_string*)vgc_heap_data_new(heap,
					  string_size,
					  string_length,
					  vgc_obj_type_string,
					  area_type);
  if(string){
    string->len = string_length;
  }
  return string;
}

void vgc_string_log(vgc_heap* heap){
  int i = 0;
  vgc_string* string;
  vgc_heap_obj_pop(heap,string,vgc_string);
  while(i < string->len){
    ulog1("string:%d",string->u.b[i]);
    i++;
  }
  vgc_heap_obj_push(heap,string);
}

vgc_subr* vgc_subr_new(vgc_heap* heap,
		       usize_t params_count,
		       usize_t locals_count,
		       int area_type){
  vgc_subr* subr;
  subr = vgc_heap_obj_new(heap,
			  vgc_subr,
			  vgc_obj_type_subr,
			  area_type);
  if(subr){
    subr->params_count = params_count;
    subr->locals_count = locals_count;
    vgc_obj_slot_set(heap,subr,consts);
    vgc_obj_slot_set(heap,subr,bytecode);
  }
  return subr;
}

vgc_call* vgc_call_new(vgc_heap* heap,
		       int call_type,
		       usize_t base){
  vgc_call* call;
  vgc_subr* subr;
  vgc_string* bytecode;
  call = vgc_heap_obj_new(heap,
			  vgc_call,
			  vgc_obj_type_call,
			  vgc_heap_area_active);
  if(call){
    call->call_type = call_type;
    call->base = base;
    if(call_type == vgc_call_type_cfun){
      vgc_obj_slot_set(heap,call,cfun);
    }else{
      vgc_heap_obj_pop(heap,subr,vgc_subr);
      vgc_obj_slot_get(heap,subr,bytecode);
      vgc_heap_obj_pop(heap,bytecode,vgc_string);
      call->pc = bytecode->u.b;
      vgc_heap_obj_push(heap,subr);
      vgc_obj_slot_set(heap,call,subr);
    }
    vgc_obj_slot_set(heap,call,caller);
  }
  return call;
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
    vslot_bool_set(bool,vbool_true);
  }else{
    vslot_bool_set(bool,vbool_false);
  }
  return bool;
}

vslot vslot_ref_eq(vslot slot1,vslot slot2){
  vgc_obj* ref1 = vslot_ref_get(slot1,vgc_obj);
  vgc_obj* ref2 = vslot_ref_get(slot2,vgc_obj);
  vslot bool;
  if(ref1 == ref2){
    vslot_bool_set(bool,vbool_true);
  }else{
    vslot_bool_set(bool,vbool_false);
  }
  return bool;
}

vgc_extend* vgc_extend_new(vgc_heap* heap,
			   int struct_size,
			   int ref_count,
			   vgc_objex_t* oet){
  vgc_extend* extend =
    (vgc_extend*)vgc_heap_data_new(heap,
				struct_size,
				ref_count,
				vgc_obj_type_extend,
				vgc_heap_area_active);
  if(extend){
    extend->oet = oet;
  }
  return extend;
}
