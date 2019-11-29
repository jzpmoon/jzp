#include <string.h>
#include "ualloc.h"
#include "uerror.h"
#include "vgc_obj.h"


int vgc_array_new(vgc_heap* heap,
		  usize_t len,
		  int area_type){
  usize_t size = TYPE_SIZE_OF(vgc_array,vslot,len);
  int i = 0;
  if(vgc_heap_data_new
     (heap,size,len,vgc_obj_type_array,area_type)){
    return -1;
  }
  while(i < len){
    vslot slot;
    vslot_null_set(slot);
    vgc_heap_stack_push(heap,slot);
    vgc_obj_set(heap,-2,i);
    i++;
  }
  return 0;
}

int vgc_string_new(vgc_heap* heap,
		   usize_t len,
		   int area_type){
  usize_t size = TYPE_SIZE_OF(vgc_string,char,len);
  return vgc_heap_data_new(heap,size,len,vgc_obj_type_string,area_type);
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
  vgc_obj_slot_set(heap,subr,bytecode);
  vgc_obj_slot_set(heap,subr,consts);
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
  vgc_obj_slot_set(heap,call,caller);
  if(call_type == vgc_call_type_cfun){
    vgc_obj_slot_set(heap,call,cfun);
  }else{
    vgc_pop_obj(heap,subr,vgc_subr);
    vgc_obj_slot_get(heap,subr,bytecode);
    vgc_pop_obj(heap,bytecode,vgc_string);
    call->pc = bytecode->u.c;
  }
  vgc_push_obj(heap,call);
  return 0;
}
