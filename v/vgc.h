#ifndef _VGC_H_
#define _VGC_H_

#include "vobj.h"
#include "ustack.h"

typedef struct _VgcHeap{
  usize_t size;
  VgcObj* rootSet;
  VgcObj* end;
  VgcObj* mem;
  ustack stack;
} VgcHeap;

void vgcCollect(VgcHeap* heap);

VgcObj* vgcHeapNew(VgcHeap* heap,
		   usize_t size,
		   enum gcObj_t ot);

#define vgcHeapNewObj(H,T)	      \
  vgcHeapNew((H),sizeof(T),gc_obj)

#define vgcHeapNewArray(H,L)	      \
  vgcHeapNew((H),		      \
	     sizeof(VgcArray)+	      \
	     sizeof(VgcObj*)*((L)-1), \
	     gc_arr)

#endif
