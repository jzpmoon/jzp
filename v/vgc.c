#include "ualloc.h"
#include "ustack_tpl.c"
#include "vgc_obj.h"

ublock_tpl(vslot)
ustack_log_tpl(vslot)
ublock_new_tpl(vslot)
ublock_push_tpl(vslot)
ustack_push_tpl(vslot)
ublock_pop_tpl(vslot)
ustack_pop_tpl(vslot)

ublock_tpl(vgc_objp)
ustack_log_tpl(vgc_objp)
ublock_new_tpl(vgc_objp)
ublock_push_tpl(vgc_objp)
ustack_push_tpl(vgc_objp)
ublock_pop_tpl(vgc_objp)
ustack_pop_tpl(vgc_objp)
  
#define vgc_obj_unmark(obj)			\
  ((obj)->_mark.m = 0)
#define vgc_obj_mark_addr(obj)			\
  ((obj)->_mark.a = 1)
#define vgc_obj_next(obj,next)			\
  (vgc_obj*)((char*)(obj) + (next)->_size)
#define vgc_obj_move(obj)			\
  memcpy(obj->_addr,obj,obj->_size)
#define vgc_obj_is_mark_addr(obj)		\
  ((obj)->_mark.a)
#define vgc_obj_unmark_addr(obj)		\
  ((obj)->_mark.a = 0)
#define vgc_obj_is_mark(obj)			\
  ((obj)->_mark.m)
#define vgc_obj_mark(obj)			\
  ((obj)->_mark.m = 1)
#define vgc_obj_slot_list(obj)						\
  (vslot*)((char*)obj + obj->_size - (sizeof(vslot) * obj->_len))
#define vgc_obj_is_static(heap,obj)					\
  ((obj) >= (heap)->area_static_begin &&				\
   (obj) < ((char*)(heap)->area_static_begin + (heap)->area_static_size))

vgc_heap* vgc_heap_new(usize_t area_static_size,
		       usize_t area_active_size,
		       usize_t root_set_size){
  usize_t align_static_size = ALIGN_BOUNDARY(area_static_size);
  usize_t align_active_size = ALIGN_BOUNDARY(area_active_size);
  usize_t area_size = align_static_size + align_active_size;
  usize_t heap_size = TYPE_SIZE_OF(vgc_heap,char,area_size);
  vgc_heap* heap;
  unew(heap,heap_size,return NULL;);
  ustack_init(vslot,&heap->root_set,1,root_set_size);
  ustack_init(vgc_objp,&heap->stack,-1,-1);
  
  heap->area_static_size = area_static_size;
  heap->area_static_begin = (vgc_obj*)heap->heap_data;
  heap->area_static_index = (vgc_obj*)heap->heap_data;
  
  heap->area_active_size = area_active_size;
  heap->area_active_begin = (vgc_obj*)heap->heap_data + align_static_size;
  heap->area_active_index = (vgc_obj*)heap->heap_data + align_static_size;
  
  heap->heap_size = heap_size;

  return heap;
}

void vgc_collect_mark(vgc_heap* heap,vgc_obj* bg_obj){
  ustack_vgc_objp* stack = &heap->stack;
  if(ustack_push_vgc_objp(stack,bg_obj)){
    uabort("vgc_collect_mark:stack overflow!");
  }
  while(1){
    vgc_obj* obj;
    if(ustack_pop_vgc_objp(stack,&obj)){
      return;
    }
    if(!obj || vgc_obj_is_mark(obj)){
      continue;
    }
    if(!vgc_obj_is_static(heap,obj)){
      vgc_obj_mark(obj);
    }
    {
      vslot* obj_slot_list = vgc_obj_slot_list(obj);
      int i = 0;
      while(i < obj->_top){
	vslot slot = obj_slot_list[i];
	if(vslot_is_ref(slot)){
	  vgc_obj* ref_obj = vslot_ref_get(slot);
	  if(ustack_push_vgc_objp(stack,ref_obj)){
	    uabort("vgc_collect_mark:stack overflow!");
	  }
	}
	i++;
      }
    }
  }
}

void vgc_collect_cal_addr(vgc_heap* heap){
  vgc_obj* moved_addr = heap->area_active_begin;
  vgc_obj* next_obj = heap->area_active_begin;
  vgc_obj* last_obj = heap->area_active_index;
  while(next_obj < last_obj){
    if(vgc_obj_is_mark(next_obj)){
      vgc_obj_mark_addr(next_obj);
      next_obj->_addr = moved_addr;
      moved_addr = vgc_obj_next(moved_addr,next_obj);
    }
    next_obj = vgc_obj_next(next_obj,next_obj);
  }
}

void vgc_collect_update_addr(vgc_heap* heap,vgc_obj* bg_obj){
  ustack_vgc_objp* stack = &heap->stack;
  if(ustack_push_vgc_objp(stack,bg_obj)){
    uabort("vgc_collect_mark:stack overflow!");
  }
  while(1){
    vgc_obj* obj;
    if(ustack_pop_vgc_objp(stack,&obj)){
      return;
    }
    if(!obj || !vgc_obj_is_mark_addr(obj)){
      continue;
    }
    vgc_obj_unmark_addr(obj);
    {
      vslot* obj_slot_list = vgc_obj_slot_list(obj);
      int i = 0;
      while(i < obj->_top){
	vslot slot = obj_slot_list[i];
	if(vslot_is_ref(slot)){
	  vgc_obj* ref_obj = vslot_ref_get(slot);
	  obj_slot_list[i] = ref_obj->_addr;
	  if(ustack_push_vgc_objp(stack,ref_obj)){
	    uabort("vgc_collect_mark:stack overflow!");
	  }
	}
	i++;
      }
    }
  }
}

vgc_obj* vgc_collect_move(vgc_heap* heap){
  vgc_obj* next_obj = heap->area_active_begin;
  vgc_obj* last_obj = heap->area_active_index;
  vgc_obj* prev_obj = next_obj;
  while(next_obj < last_obj){
    if(vgc_obj_is_mark(next_obj)){
      vgc_obj_unmark(next_obj);
      vgc_obj_move(next_obj);
      prev_obj = next_obj->_addr;
    }
    next_obj = vgc_obj_next(next_obj,next_obj);
  }
  return vgc_obj_next(prev_obj,prev_obj);
}

int vgc_collect(vgc_heap* heap){
  ustack_vslot* root_set = &heap->root_set;
  ublock_vslot* block = root_set->curr_block;
  int i;
  if(!block){
    return 0;
  }
  for(i = 0;i < root_set->block_pos;i++){
    vslot slot = block->ptr[i];
    if(vslot_is_ref(slot)){
      vgc_obj* obj = vslot_ref_get(slot);
      vgc_collect_mark(heap,obj);
    }
  }

  vgc_collect_cal_addr(heap);

  for(i = 0;i < root_set->block_pos;i++){
    vslot slot = block->ptr[i];
    if(vslot_is_ref(slot)){
      vgc_obj* obj = vslot_ref_get(slot);
      vgc_collect_update_addr(heap,obj);
    }
  }

  heap->area_active_index = vgc_collect_move(heap);
  
  return 0;
}

int vgc_heap_try_data_new(vgc_heap* heap,
			  usize_t obj_size,
			  usize_t ref_count,
			  int area_type){
  vgc_obj* index_obj;
  vgc_obj* next_obj;
  vgc_obj* last_obj;
  usize_t align_obj_size;
  if(area_type == vgc_heap_area_static){
    index_obj = heap->area_static_begin;
    area_size = heap->area_static_size;
  }else{
    index_obj = heap->area_active_begin;
    area_size = heap->area_active_size;
  }
  align_obj_size = ALIGN_BOUNDARY(obj_size);
  next_obj = vgc_obj_next2(index_obj,align_obj_size);
  last_obj = vgc_obj_next2(index_obj,area_size);
  if(next_obj <= last_obj){
    
  }
  return 0;
}

int vgc_heap_data_new(vgc_heap* heap,
		      usize_t obj_size,
		      usize_t ref_count,
		      int area_type){
  return 0;
}
