#include <string.h>
#include "ualloc.h"
#include "uerror.h"
#include "vgc_obj.h"

vgc_heap* vgc_heap_new(usize_t static_size,
		       usize_t active_size) {
  usize_t align_static_size = ALIGN_BOUNDARY(static_size);
  usize_t align_active_size = ALIGN_BOUNDARY(active_size);
  usize_t align_memory_size = align_static_size + align_active_size;
  ustack stack = {NULL,NULL,0,0,4};
  void* memory;
  vgc_heap* heap;
  unew(memory,align_memory_size,return NULL;);
  unew(heap,sizeof(vgc_heap),goto error;);
  heap->static_begin = memory;
  heap->static_end   = memory + align_static_size;
  heap->static_index = heap->static_begin;
  heap->active_begin = heap->static_end;
  heap->active_end   = memory + align_memory_size;
  heap->active_index = heap->active_begin;
  heap->memory_begin = memory;
  heap->memory_end   = heap->active_end;
  heap->stack = stack;
  return heap;
 error:
  ufree(memory);
  return NULL;
}

#define _IS_MARK(O)    ((O)->mark.m)
#define _MARK_OBJ(O)   ((O)->mark.m=1)
#define _UNMARK(O)     ((O)->mark.m=0)
#define _IS_ADDR(O)    ((O)->mark.a)
#define _MARK_ADDR(O)  ((O)->mark.a=1)
#define _UNADDR(O)     ((O)->mark.a=0)
#define _INITMARK(O,T) \
  (O)->mark.m=0;       \
  (O)->mark.a=0;       \
  (O)->mark.t=(T)

#define _IS_STATIC(H,O)					\
  ((O) >= (H)->static_begin && (O) < (H)->static_end)

#define _NEXT_OBJ(O,S)				        \
  ((vgc_obj*)((void*)(O) + ALIGN_BOUNDARY(S)))

#define _LAST_OBJ(H) ((H)->memory_end)

void mark_obj(vgc_obj* root,ustack* stack) {
  if(ustack_push_obj(stack,root))
    uabort("vgc_heap:stack overflow!");
  while(1){
    vslot* ref_list;
    vgc_obj* tmp_obj;
    usize_t i;
    if(ustack_pop_obj(stack,tmp_obj)){
      return;
    }
    if(!tmp_obj||_IS_MARK(tmp_obj)){
      continue;
    }
    ulog("*********mark*********");
    vgc_obj_log(tmp_obj);
    _MARK_OBJ(tmp_obj);
    ref_list = vgc_obj_ref_list(tmp_obj);
    for(i=0;i<tmp_obj->top;i++){
      vgc_obj* ref_obj = vslot_ref_get(ref_list[i]);
      if(ustack_push_obj(stack,ref_obj)){
	uabort("vgc_heap:stack overflow!");
      }
    }
  }
}

void cal_addr(vgc_obj* bg_obj,vgc_obj* ed_obj) {
  vgc_obj* tmp_obj  = bg_obj;
  vgc_obj* new_addr = bg_obj;
  while(tmp_obj < ed_obj){
    if(_IS_MARK(tmp_obj)){
      ulog("*********cal*********");
      vgc_obj_log(tmp_obj);
      _MARK_ADDR(tmp_obj);
      tmp_obj->addr = new_addr;
      new_addr = _NEXT_OBJ(new_addr,tmp_obj->size);
    }
    tmp_obj = _NEXT_OBJ(tmp_obj,tmp_obj->size);
  }
}

void update_addr(vgc_heap* heap,vgc_obj* root,ustack* stack) {
  if(ustack_push_obj(stack,root))
    uabort("vgc_heap:stack overflow!");
  while(1){
    vslot* ref_list;
    vgc_obj* tmp_obj;
    usize_t i;
    if(ustack_pop_obj(stack,tmp_obj)){
      return;
    }
    if(!tmp_obj || !_IS_MARK(tmp_obj)){
      continue;
    }
    ulog("*********update*********");
    vgc_obj_log(tmp_obj);
    _UNMARK(tmp_obj);
    ref_list=vgc_obj_ref_list(tmp_obj);
    for(i=0;i < tmp_obj->top;i++){
      vgc_obj* ref_obj = vslot_ref_get(ref_list[i]);
      if(ref_obj && !_IS_STATIC(heap,ref_obj)){
	vslot_ref_set(ref_list[i],ref_obj->addr);
      }
      if(ustack_push_obj(stack,ref_obj)){
	uabort("vgc_heap:stack overflow!");
      }
    }
  }
}

vgc_obj* move_obj(vgc_obj* bg_obj,vgc_obj* ed_obj) {
  vgc_obj* tmp_obj   = bg_obj;
  vgc_obj* index_obj = bg_obj;
  while(tmp_obj < ed_obj){
    vgc_obj* next_obj = _NEXT_OBJ(tmp_obj,tmp_obj->size);
    if(_IS_ADDR(tmp_obj)){
      ulog("*********move*********");
      vgc_obj_log(tmp_obj);
      _UNADDR(tmp_obj);
      memcpy(tmp_obj->addr,tmp_obj,tmp_obj->size);
      index_obj = tmp_obj->addr;
    }else{
      ulog("*********release*********");
      vgc_obj_log(tmp_obj);
    }
    tmp_obj=next_obj;
  }
  return index_obj;
}

void vgc_collect(vgc_heap* heap) {
  vgc_obj* root;
  ulog("vgc_heap:Collect start");
  
  root = heap->static_begin;
  while(root < heap->static_index){
    mark_obj(root,&heap->stack);
    root = _NEXT_OBJ(root,root->size);
  }
  ulog("vgc_heap:Mark object end");
  
  cal_addr(heap->active_begin,heap->active_index);
  ulog("vgc_heap:Cal Address end");
  
  root = heap->static_begin;
  while(root < heap->static_index){
    update_addr(heap,root,&heap->stack);
    root = _NEXT_OBJ(root,root->size);
  }
  ulog("vgc_heap:Update Address end");
  
  heap->active_index =
    move_obj(heap->active_begin,heap->active_index);
  ulog("vgc_heap:Collect end");
}

vgc_obj* vgc_try_new(vgc_heap* heap,
		     usize_t   size,
		     usize_t   len,
		     int       obj_type,
		     int       area_type) {
  vgc_obj* new_obj = NULL;
  vgc_obj* next_obj;
  vgc_obj* last_obj;
  if (area_type == area_static) {
    next_obj = _NEXT_OBJ(heap->static_index,size);
    last_obj = heap->static_end;
    if (next_obj <= last_obj) {
      new_obj = heap->static_index;
      new_obj->addr = NULL;
      new_obj->size = size;
      new_obj->len  = len;
      new_obj->top  = 0;
      _INITMARK(new_obj,obj_type);
      heap->static_index = next_obj;
    }
  } else {
    next_obj = _NEXT_OBJ(heap->active_index,size);
    last_obj = heap->active_end;
    if (next_obj <= last_obj) {
      new_obj = heap->active_index;
      new_obj->addr = NULL;
      new_obj->size = size;
      new_obj->len  = len;
      new_obj->top  = 0;
      _INITMARK(new_obj,obj_type);
      heap->active_index = next_obj;
    }
  }
  return new_obj;
}

vgc_obj* _vgc_heap_obj_new(vgc_heap* heap,
			   usize_t   size,
			   usize_t   len,
			   int       obj_type,
			   int       area_type) {
  vgc_obj* new_obj = vgc_try_new(heap,size,len,obj_type,area_type);
  if(!new_obj && area_type != area_static){
    vgc_collect(heap);
    new_obj = vgc_try_new(heap,size,len,obj_type,area_type);
  }
  return new_obj;
}
