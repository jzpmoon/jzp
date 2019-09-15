#ifndef _VGC_OBJ_H_
#define _VGC_OBJ_H_

#include "udef.h"
#include "ustack.h"

enum {
    gc_stack,
      gc_num,
      gc_str,
    gc_cfunc,
     gc_subr,
     gc_call,
  gc_context,
};

#define VGCTYPEOF(O,T) \
  (((O)->mark.t==(T)))

struct mark_t{
  unsigned char m : 1;
  unsigned char a : 1;
  unsigned char t : 6;
};

#define VGCHEADER	 \
  struct _vgc_obj* addr; \
  usize_t size;	         \
  usize_t len;		 \
  usize_t top;		 \
  struct mark_t mark;

typedef struct _vgc_obj{
  VGCHEADER;
} vgc_obj;

enum {
  vsolt_type_num,
  vsolt_type_bool,
  vsolt_type_ptr,
};

typedef struct _vsolt{
  int t;
  union {
    double number;
    int boolean;
    vgc_obj* ptr;
  } u;
} vsolt;

typedef struct _vgc_stack{
  VGCHEADER;
  vgc_obj* objs[1];
} vgc_stack;

enum {
  area_static,
  area_active,
};

typedef struct _vgc_heap{
  vgc_obj* static_begin;
  vgc_obj*   static_end;
  vgc_obj* static_index;
  vgc_obj* active_begin;
  vgc_obj*   active_end;
  vgc_obj* active_index;
  vgc_obj* memory_begin;
  vgc_obj*   memory_end;
  ustack          stack;
} vgc_heap;

vgc_heap* vgc_heap_new(usize_t static_size,
		       usize_t active_size);

void vgc_collect(vgc_heap* heap);

vgc_obj*
_vgc_heap_obj_new(vgc_heap* heap,
		  usize_t   size,
		  usize_t    len,
		  int   obj_type,
		  int  area_type);

#define vgc_heap_obj_new(heap,size,len,obj_type,area_type)	\
  _vgc_heap_obj_new(heap,sizeof(size),len,obj_type,area_type)

#define vgc_obj_ref_list(obj)				\
  ((vgc_obj**)(((void*)(obj)+(obj)->size)-sizeof(vgc_obj*)*(obj)->len))

#define vgc_obj_ref_check(obj,index) 		\
  ((index)>=0&&(index)<(obj)->len)

#define vgc_obj_is_full(obj)			\
  ((obj)->top>=(obj)->len)

#define vgc_obj_is_empty(obj)			\
  ((obj)->top<=0)

#define vgc_obj_flip(obj) \
  (obj)->top=(obj)->len

vgc_stack*
vgc_stack_new(vgc_heap* heap,
	      usize_t    len);

void vgc_stack_push(vgc_stack* stack,
		    vgc_obj*     obj);

vgc_obj*
vgc_stack_pop(vgc_stack* stack);

vgc_stack*
vgc_stack_expand(vgc_heap*   heap,
		 vgc_stack* stack);

typedef struct _vgc_str{
  VGCHEADER;
  usize_t str_len;
  union {
    char c[1];
    unsigned char b[1];
  } u;
} vgc_str;

vgc_str*
vgc_str_new(vgc_heap*  heap,
	    usize_t str_len);

void vgc_str_log(vgc_str* str);

#define vgc_str_bound_check(str,index) \
  ((index)>=0&&(index)<(str)->str_len)

typedef struct _vgc_subr{
  VGCHEADER;
  usize_t para_len;
  usize_t local_len;
  vgc_str* bc;
  vgc_stack* consts;
} vgc_subr;

vgc_subr*
vgc_subr_new(vgc_heap* heap,
	     usize_t para_len,
	     usize_t local_len,
	     vgc_str* bc,
	     vgc_stack* consts);

typedef struct _vgc_call{
  VGCHEADER;
  unsigned char* pc;
  usize_t base;
  vgc_subr* subr;
  vgc_stack* locals;
  struct _vgc_call* caller;
} vgc_call;

vgc_call*
vgc_call_new(vgc_heap*    heap,
	     usize_t      base,
	     vgc_subr*    subr,
	     vgc_stack* locals,
	     vgc_call*  caller);

typedef struct _vgc_num{
  VGCHEADER;
  float value;
} vgc_num,vgc_bool;

vgc_num*
vgc_num_new(vgc_heap* heap,
	    float value);

vgc_num*
vgc_num_add(vgc_heap* heap,
	    vgc_num* num1,
	    vgc_num* num2);

vgc_bool*
vgc_num_eq(vgc_heap* heap,
	   vgc_num* num1,
	   vgc_num* num2);

#define VTRUE (1)
#define VFALSE (0)
#define VTRUEP(B) ((B)->value)
#define VFALSEP(B) (!(B)->value)
#define vgc_true_new(heap) \
  vgc_num_new(heap,VTRUE)
#define vgc_false_new(heap) \
  vgc_num_new(heap,VFALSE)

#endif
