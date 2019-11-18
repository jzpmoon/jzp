#include <string.h>
#include "ualloc.h"
#include "uerror.h"
#include "vgc_obj.h"


int vgc_array_new(vgc_heap* heap,
		  usize_t len,
		  int area_type){
  usize_t size = TYPE_SIZE_OF(vgc_array,vslot,len);
  return vgc_heap_data_new(heap,size,len,vgc_obj_type_array,area_type);
}

int vgc_string_new(vgc_heap* heap,
		   usize_t len,
		   int area_type){
  usize_t size = TYPE_SIZE_OF(vgc_string,char,len);
  return vgc_heap_data_new(heap,size,len,vgc_obj_type_string,area_type);
}
