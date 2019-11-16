#include "ualloc.h"
#include "ustack_tpl.c"
#include "vgc_obj.h"

ublock_tpl(vslot)
ustack_log_tpl(vslot)
ublock_push_tpl(vslot)
ustack_push_tpl(vslot)
ublock_pop_tpl(vslot)
ustack_pop_tpl(vslot)

ublock_tpl(vgc_objp)
ustack_log_tpl(vgc_objp)
ublock_push_tpl(vgc_objp)
ustack_push_tpl(vgc_objp)
ublock_pop_tpl(vgc_objp)
ustack_pop_tpl(vgc_objp)

vgc_heap* vgc_heap_new(usize_t area_static_size,
		       usize_t area_active_size){
  usize_t align_static_size = ALIGN_BOUNDARY(area_static_size);
  usize_t align_active_size = ALIGN_BOUNDARY(area_active_size);
  usize_t area_size = align_static_size + align_active_size;
  usize_t heap_size = TYPE_SIZE_OF(vgc_heap,char,area_size);
  vgc_heap* heap;
  unew(heap,heap_size,return NULL;);
  ustack_init(vslot,heap->root_set,1);
  ustack_init(vgc_objp,heap->stack,-1);
  
  heap->area_static_size = area_static_size;
  heap->area_static_begin = (vgc_obj*)heap->heap_data;
  heap->area_static_index = (vgc_obj*)heap->heap_data;
  
  heap->area_active_size = area_active_size;
  heap->area_active_begin = (vgc_obj*)heap->heap_data + align_static_size;
  heap->area_active_index = (vgc_obj*)heap->heap_data + align_static_size;
  
  heap->heap_size = heap_size;

  return heap;
}

int vgc_heap_data_new(vgc_heap* heap,
		      usize_t obj_size,
		      usize_t ref_count,
		      int area_type){
  return 0;
}
