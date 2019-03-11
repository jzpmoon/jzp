#include <string.h>
#include "vgc.h"
#include "ustack.h"
#include "ualloc.h"

#define _NEXT_OBJ(O,S)		\
  ((VgcObj*)((void*)(O)+        \
	     ALIGN_BOUNDARY(S)))

void markObj(VgcObj* obj,
	     ustack* stack){
  if(ustack_push_obj(stack,obj))
    uabort();
  while(1){
    VgcObj* tmp;
    if(ustack_pop_obj(stack,tmp))
      return;
    if(!tmp||ISMARK(tmp))
      continue;
    GCMARK(tmp);
    if(GCTYPEOF(tmp,gc_arr)){
      VgcArray* array=(VgcArray*)tmp;
      usize_t i;
      for(i=0;i<array->len;i++){
	if(ustack_push_obj
	   (stack,array->objs[i]))
	  uabort();
      }
    }
  }
}

void calAddr(VgcObj* bgObj,
	     VgcObj* edObj){
  VgcObj* tmp=bgObj;
  VgcObj* addr=bgObj;
  while(tmp<edObj){
    if(ISMARK(tmp)){
      MARKADDR(tmp);
      tmp->addr=addr;
      addr=_NEXT_OBJ(addr,tmp->size);
    }
    tmp=_NEXT_OBJ(tmp,tmp->size);
  }
}

VgcObj* updateAddr(VgcObj* obj,
		   ustack* stack){
  if(ustack_push_obj(stack,obj))
    uabort();
  while(1){
    VgcObj* tmp;
    if(ustack_pop_obj(stack,tmp)){
      if(obj)
	return obj->addr;
      else
	return NULL;
    }
    if(!tmp||!ISADDR(tmp))
      continue;
    UNADDR(tmp);
    if(GCTYPEOF(tmp,gc_arr)){
      VgcArray* array=(VgcArray*)tmp;
      usize_t i;
      for(i=0;i<array->len;i++){
	VgcObj* o=array->objs[i];
	array->objs[i]=o->addr;
	if(ustack_push_obj(stack,o))
	  uabort();
      }
    }
  }
}

VgcObj* moveObj(VgcObj* bgObj,
	     VgcObj* edObj){
  VgcObj* tmp=bgObj;
  VgcObj* addr=bgObj;
  while(tmp<edObj){
    if(ISMARK(tmp)){
      UNMARK(tmp);
      memcpy(tmp->addr,tmp,tmp->size);
      addr=_NEXT_OBJ(addr,tmp->size);
    }
    tmp=_NEXT_OBJ(tmp,tmp->size);
  }
  return addr;
}

void vgcCollect(VgcHeap* heap){
  
  markObj(heap->rootSet,
	  &heap->stack);

  calAddr(heap->mem,
	  heap->end);

  heap->rootSet=
    updateAddr(heap->rootSet,
	       &heap->stack);
  
  heap->end=moveObj(heap->mem,
		    heap->end);

}

VgcObj* vgcTryNew(VgcHeap* heap,
		  usize_t size,
		  enum gcObj_t ot){
  VgcObj* newObj=NULL;
  if(((void*)heap->end+size)<=
     ((void*)heap->mem+heap->size)){
    newObj=heap->end;
    INITMARK(newObj,ot);
    newObj->addr=NULL;
    newObj->size=size;
    heap->end=_NEXT_OBJ(heap->end,size);
  }
  return newObj;
}

VgcObj* vgcHeapNew(VgcHeap* heap,
		   usize_t size,
		   enum gcObj_t ot){
  VgcObj* newObj=vgcTryNew(heap,
			   size,
			   ot);
  if(!newObj){
    vgcCollect(heap);
    newObj=vgcTryNew(heap,size,ot);
  }
  return newObj;
}
