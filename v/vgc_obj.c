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
  usize_t i = 0;
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

void vgc_array_set(vgc_array* array,usize_t idx,vslot slot){
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
					  0,
					  vgc_obj_type_string,
					  area_type);
  if(string){
    string->len = string_length;
  }
  return string;
}

void vgc_ustr2vstr(vgc_string* vstr,ustring* ustr)
{
  memcpy(vstr->u.b,ustr->value,ustr->len);
}

vgc_ref* vgc_ref_new(vgc_heap* heap,vslot slot)
{
  vgc_ref* ref;
  ref = vgc_heap_obj_new(heap,vgc_ref,vgc_obj_type_ref,vgc_heap_area_static);
  if (ref) {
    vgc_slot_set(ref,ref,slot);
  }
  return ref;
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

vslot vslot_int_eq(vslot slot1,vslot slot2){
  int int1 = vslot_int_get(slot1);
  int int2 = vslot_int_get(slot2);
  vslot bool;
  if(int1 == int2){
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
