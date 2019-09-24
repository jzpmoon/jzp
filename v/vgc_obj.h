#ifndef _VGC_OBJ_H_
#define _VGC_OBJ_H_

#include "udef.h"
#include "ustack.h"
#include "ustring_table.h"

enum {
  gc_stack,
  gc_str,
  gc_cfunc,
  gc_subr,
  gc_call,
  gc_context,
  gc_extend,
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
  area_static,
  area_active,
};

typedef struct _vgc_heap{
  vgc_obj* static_begin;
  vgc_obj* static_end;
  vgc_obj* static_index;
  vgc_obj* active_begin;
  vgc_obj* active_end;
  vgc_obj* active_index;
  vgc_obj* memory_begin;
  vgc_obj* memory_end;
  ustack   stack;
} vgc_heap;

vgc_heap* vgc_heap_new(usize_t static_size,
		       usize_t active_size);

void vgc_collect(vgc_heap* heap);

vgc_obj*
_vgc_heap_obj_new(vgc_heap* heap,
		  usize_t   size,
		  usize_t   len,
		  int       obj_type,
		  int       area_type);

#define vgc_heap_obj_new(heap,stype,slen,otype,atype)			\
  (stype*)_vgc_heap_obj_new(heap,sizeof(stype),slen,otype,atype)

#define vgc_obj_ref_list(obj)						\
  ((vslot*)(((void*)(obj)+(obj)->size)-sizeof(vslot)*(obj)->len))

#define vgc_obj_ref_check(obj,index)		\
  ((index)>=0&&(index)<(obj)->len)

#define vgc_obj_is_full(obj)			\
  ((obj)->top>=(obj)->len)

#define vgc_obj_is_empty(obj)			\
  ((obj)->top<=0)

#define vgc_obj_flip(obj)			\
  (obj)->top=(obj)->len

void vgc_obj_log(vgc_obj* obj);

enum {
  vslot_type_null,
  vslot_type_num,
  vslot_type_bool,
  vslot_type_ref,
};

typedef struct _vslot{
  int t;
  union {
    double   num;
    int      bool;
    vgc_obj* ref;
  } u;
} vslot;

vslot vslot_num_add(vslot  num1,
		    vslot  num2);

vslot vslot_num_eq(vslot  num1,
		   vslot  num2);

vslot vslot_ref_eq(vslot ref1,
		   vslot ref2);

#define VTRUE (1)

#define VFALSE (0)

#define VTRUEP(_bool) ((_bool).u.bool)

#define VFALSEP(_bool) (!(_bool).u.bool)

#define VSLOT_NULL ((vslot){vslot_type_null,{0}})

#define vslot_is_ref(slot) ((slot).t==vslot_type_ref)

#define vslot_is_null(slot) ((slot).t==vslot_type_null)

#define vslot_is_num(slot) ((slot).t==vslot_type_num)

#define vslot_is_bool(slot) ((slot).t==vslot_type_bool)

#define vslot_ref_get(slot)			                \
  (((slot).t==vslot_type_ref)?(slot).u.ref:NULL)

#define vslot_num_get(slot)			                \
  ((slot).u.num)

#define vslot_bool_get(slot)			                \
  ((slot).u.bool)

#define vslot_ref_set(slot,obj)					\
  ((slot).t=vslot_type_ref,(slot).u.ref=(vgc_obj*)(obj))

#define vslot_num_set(slot,_num)		                \
  ((slot).t=vslot_type_num,(slot).u.num=(_num))

#define vslot_bool_set(slot,_bool)			        \
  ((slot).t=vslot_type_bool,(slot).u.bool=(_bool))

typedef struct _vgc_stack{
  VGCHEADER;
  vslot objs[1];
} vgc_stack;

vgc_stack* vgc_stack_new(vgc_heap* heap,
			 usize_t   len);

void vgc_stack_push(vgc_stack* stack,
		    vslot      slot);

vslot vgc_stack_pop(vgc_stack* stack);

vgc_stack* vgc_stack_expand(vgc_heap*  heap,
			    vgc_stack* stack);

typedef struct _vgc_str{
  VGCHEADER;
  usize_t str_len;
  union {
    char c[1];
    unsigned char b[1];
  } u;
} vgc_str;

vgc_str* vgc_str_new(vgc_heap*  heap,
		     usize_t    str_len);

vgc_str* vgc_str_newc(vgc_heap*  heap,
		      char*      charp,
		      usize_t    str_len);

void vgc_str_log(vgc_str* str);

#define vgc_str_bound_check(str,index) \
  ((index)>=0&&(index)<(str)->str_len)

typedef struct _vgc_subr{
  VGCHEADER;
  usize_t para_len;
  usize_t local_len;
  /*vgc_str*/
  vslot bc;
  /*vgc_stack*/
  vslot consts;
} vgc_subr;

vgc_subr*
vgc_subr_new(vgc_heap*  heap,
	     usize_t    para_len,
	     usize_t    local_len,
	     vgc_str*   bc,
	     vgc_stack* consts);

typedef struct _vgc_call{
  VGCHEADER;
  unsigned char* pc;
  usize_t base;
  /*vgc_subr*/
  vslot subr;
  /*vgc_stack*/
  vslot locals;
  /*vgc_call*/
  vslot caller;
} vgc_call;

vgc_call*
vgc_call_new(vgc_heap*  heap,
	     usize_t    base,
	     vgc_subr*  subr,
	     vgc_stack* locals,
	     vgc_call*  caller);

typedef struct _vgc_objex_t{
  ustring* type_name;
} vgc_objex_t;

#define VGCHEADEREX \
  VGCHEADER	    \
  vgc_objex_t oet;

typedef struct _vgc_objex{
  VGCHEADEREX
} vgc_objex;

vgc_objex* _vgc_objex_new(vgc_heap*   heap,
			  usize_t     objex_size,
			  usize_t     slot_len,
			  vgc_objex_t objex_type,
			  int         area_type);

#define vgc_objex_new(heap,stype,slen,otype,atype)		\
  (stype*)_vgc_objex_new(heap,sizeof(stype),slen,otype,atype)

ustring* vstrtb_put(char* charp);

vgc_objex_t* vgc_objex_init(char* str,int len);

#define vgc_objex_is_init(otype) ((otype) != NULL)

#endif
