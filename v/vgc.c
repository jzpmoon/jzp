#include <string.h>
#include "ualloc.h"
#include "ustack_tpl.c"
#include "vgc_obj.h"

ustack_def_tpl(vslot)
ustack_def_tpl(vgc_objp)
  
#define vgc_obj_unmark(obj)			\
  ((obj)->_mark.m = 0)
#define vgc_obj_mark_addr(obj)			\
  ((obj)->_mark.a = 1)
#define vgc_obj_jump(obj,size)			\
  ((vgc_obj*)((char*)(obj) + size))
#define vgc_obj_next(obj,next)			\
  vgc_obj_jump(obj,next->_size)
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
#define vgc_area_of_obj(area,obj)				\
  ((obj) >= (area)->area_begin &&				\
   (obj) < (vgc_obj*)((char*)(area)->area_begin + (area)->area_size))
#define vgc_obj_is_static(heap,obj)		\
  vgc_area_of_obj(&heap->area_static,obj)

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
  
  heap->area_static.area_size = area_static_size;
  heap->area_static.area_begin = (vgc_obj*)heap->heap_data;
  heap->area_static.area_index = (vgc_obj*)heap->heap_data;
  
  heap->area_active.area_size = area_active_size;
  heap->area_active.area_begin =
    (vgc_obj*)(heap->heap_data + align_static_size);
  heap->area_active.area_index =
    (vgc_obj*)(heap->heap_data + align_static_size);
  
  heap->heap_size = heap_size;

  return heap;
}

void vgc_collect_mark(vgc_heap* heap,vgc_obj* begin_obj){
  ustack_vgc_objp* stack = &heap->stack;
  int mark_count = 0;
  if(ustack_push_vgc_objp(stack,begin_obj)){
    uabort("vgc_collect_mark:stack overflow!");
  }
  while(1){
    vgc_obj* obj;
    if(ustack_pop_vgc_objp(stack,&obj)){
      ulog1("vgc_collect: mark_count %d",mark_count);
      return;
    }
    if(!obj || vgc_obj_is_mark(obj)){
      continue;
    }
    if(!vgc_obj_is_static(heap,obj)){
      vgc_obj_mark(obj);
      vgc_obj_log(obj);
      mark_count++;
    }
    {
      vslot* obj_slot_list = vgc_obj_slot_list(obj);
      int i = 0;
      while(i < obj->_len){
	vslot slot = obj_slot_list[i];
	if(vslot_is_ref(slot)){
	  vgc_obj* ref_obj = vslot_ref_get(slot,vgc_obj);
	  if(ustack_push_vgc_objp(stack,ref_obj)){
	    uabort("vgc_collect_mark:stack overflow!");
	  }
	}
	i++;
      }
    }
  }
}

#define vgc_obj_move_len(obj) ((char*)(obj) - (char*)(obj)->_addr)

void vgc_collect_cal_addr(vgc_heap* heap){
  vgc_heap_area* heap_area = &heap->area_active;
  vgc_obj* move_addr = heap_area->area_begin;
  vgc_obj* next_obj = heap_area->area_begin;
  vgc_obj* last_obj = heap_area->area_index;
  while(next_obj < last_obj){
    if(vgc_obj_is_mark(next_obj)){
      vgc_obj_mark_addr(next_obj);
      next_obj->_addr = move_addr;
      move_addr = vgc_obj_next(move_addr,next_obj);
      ulog1("vgc_collect:cal addr move len %ld",
	    vgc_obj_move_len(next_obj));
    }
    next_obj = vgc_obj_next(next_obj,next_obj);
  }
}

void vgc_collect_update_addr(vgc_heap* heap,vgc_obj* begin_obj){
  ustack_vgc_objp* stack = &heap->stack;
  if(ustack_push_vgc_objp(stack,begin_obj)){
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
      while(i < obj->_len){
	vslot slot = obj_slot_list[i];
	if(vslot_is_ref(slot)){
	  vgc_obj* ref_obj = vslot_ref_get(slot,vgc_obj);
	  vslot_ref_set(obj_slot_list[i],ref_obj->_addr);
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
  vgc_heap_area* heap_area = &heap->area_active;
  vgc_obj* move_addr = heap_area->area_begin;
  vgc_obj* next_obj = heap_area->area_begin;
  vgc_obj* last_obj = heap_area->area_index;
  vgc_obj* temp_obj;
  int free_count = 0;
  while(next_obj < last_obj){
    if(vgc_obj_is_mark(next_obj)){
      ulog1("vgc_collect:move obj len %ld",
	    vgc_obj_move_len(next_obj));
      vgc_obj_log(next_obj);
      vgc_obj_unmark(next_obj);
      move_addr = vgc_obj_next(move_addr,next_obj);
      temp_obj = vgc_obj_next(next_obj,next_obj);
      if(next_obj != next_obj->_addr){
	vgc_obj_move(next_obj);
      }
      next_obj = temp_obj;
    }else{
      next_obj = vgc_obj_next(next_obj,next_obj);
      ulog("vgc_collect:free obj");
      vgc_obj_log(next_obj);
      free_count++;
    }
  }
  ulog1("vgc_collect:free count %d",free_count);
  return move_addr;
}

int vgc_collect(vgc_heap* heap){
  ustack_vslot* root_set = &heap->root_set;
  ublock_vslot* block = root_set->curr_block;
  int i;
  if(!block){
    return 0;
  }
  
  ulog("vgc_collect: mark");
  for(i = 0;i < root_set->block_pos;i++){
    vslot slot = block->ptr[i];
    if(vslot_is_ref(slot)){
      vgc_obj* obj = vslot_ref_get(slot,vgc_obj);
      vgc_collect_mark(heap,obj);
    }
  }

  ulog("vgc_collect: cal addr");
  vgc_collect_cal_addr(heap);

  ulog("vgc_collect: update addr");
  for(i = 0;i < root_set->block_pos;i++){
    vslot slot = block->ptr[i];
    if(vslot_is_ref(slot)){
      vgc_obj* obj = vslot_ref_get(slot,vgc_obj);
      vslot_ref_set(block->ptr[i],obj->_addr);
      vgc_collect_update_addr(heap,obj);
    }
  }

  ulog("vgc_collect: move");
  heap->area_active.area_index = vgc_collect_move(heap);
  
  return 0;
}

#define vgc_obj_init(obj,size,len,type)		\
  do{						\
    (obj)->_addr = NULL;			\
    (obj)->_size = size;			\
    (obj)->_len = len;				\
    (obj)->_mark.m = 0;				\
    (obj)->_mark.a = 0;				\
    (obj)->_mark.t = type;			\
  } while(0)

#define vgc_area_remain(area)				\
  ((char*)(area)->area_begin + (area)->area_size -	\
   (char*)(area)->area_index)

int vgc_heap_data_try_new(vgc_heap* heap,
			  usize_t obj_size,
			  usize_t ref_count,
			  int obj_type,
			  int area_type){
  vgc_heap_area* heap_area;
  vgc_obj* next_obj;
  vgc_obj* last_obj;
  vgc_obj* new_obj;
  usize_t align_obj_size;
  if(area_type == vgc_heap_area_static){
    heap_area = &heap->area_static;
  }else{
    heap_area = &heap->area_active;
  }
  align_obj_size = ALIGN_BOUNDARY(obj_size);
  next_obj = vgc_obj_jump(heap_area->area_index,
			  align_obj_size);
  last_obj = vgc_obj_jump(heap_area->area_begin,
			  heap_area->area_size);
  ulog1("require memory:%d",align_obj_size);
  ulog1("remain memory :%ld",vgc_area_remain(heap_area));
  if(next_obj <= last_obj){
    vslot slot;
    new_obj = heap_area->area_index;
    vgc_obj_init(new_obj,align_obj_size,ref_count,obj_type);
    heap_area->area_index = next_obj;
    vslot_ref_set(slot,new_obj);
    vgc_heap_stack_push(heap,slot);
    return 0;
  }else{
    return -1;
  }
}

int vgc_heap_data_new(vgc_heap* heap,
		      usize_t obj_size,
		      usize_t ref_count,
		      int obj_type,
		      int area_type){
  if(vgc_heap_data_try_new
     (heap,obj_size,ref_count,obj_type,area_type)){
    vgc_collect(heap);
    return vgc_heap_data_try_new
      (heap,obj_size,ref_count,obj_type,area_type);
  }
  return 0;
}

vslot vgc_heap_stack_get(vgc_heap* heap,usize_t index){
  ustack_vslot* root_set = &heap->root_set;
  if(index < -root_set->block_pos || index >= root_set->block_pos){
    uabort("vgc_heap_stack:index over of bound!");
  }
  if(index > 0){
    return root_set->curr_block->ptr[index];
  }
  return root_set->curr_block->ptr[root_set->block_pos + index - 1];
}

void vgc_heap_stack_set(vgc_heap* heap,usize_t index,vslot slot){
  ustack_vslot* root_set = &heap->root_set;
  if(index < 0 || index >= root_set->block_pos){
    uabort("vgc_heap_stack:index over of bound!");
  }
  root_set->curr_block->ptr[index] = slot;
}

int vgc_heap_stack_top_get(vgc_heap* heap){
  ustack_vslot* root_set = &heap->root_set;
  return root_set->block_pos;
}

void vgc_heap_stack_top_set(vgc_heap* heap,usize_t index){
  ustack_vslot* root_set = &heap->root_set;
  if(index < 0 || index > root_set->block_pos){
    uabort("vgc_heap_stack:index over of bound!");
  }
  root_set->block_pos = index;
}
