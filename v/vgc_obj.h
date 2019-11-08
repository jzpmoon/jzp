#ifndef _VGC_OBJ_H_
#define _VGC_OBJ_H_

#include "udef.h"
#include "ustack.h"
#include "ustring_table.h"
#include "ubuffer.h"

enum {
  gc_stack,
  gc_str,
  gc_cfun,
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

typedef struct _vslot{
  int t;
  union {
    double   num;
    int      bool;
    vgc_obj* ref;
  } u;
} vslot;

typedef struct _vgc_stack{
  VGCHEADER;
  vslot objs[1];
} vgc_stack;

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
_vgc_heap_obj_new(vgc_heap*  heap,
		  vgc_stack* stack,
		  usize_t    size,
		  usize_t    len,
		  int        obj_type,
		  int        area_type);

#define vgc_heap_obj_new(heap,stack,stype,slen,otype,atype)		\
  (stype*)_vgc_heap_obj_new(heap,stack,sizeof(stype),slen,otype,atype)

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

vslot* vslot_num_new(vgc_stack* stack,
		     double num);

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
  ((void*)(slot).u.ref)

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
  
void vslot_log(vslot slot);

vgc_stack* vgc_stack_new(vgc_heap*  heap,
			 vgc_stack* stack,
			 usize_t    len,
			 int        area_type);

int vgc_stack_push(vgc_stack* stack,
		   vslot      slot);

vslot vgc_stack_pop(vgc_stack* stack);

vslot* vgc_stack_top_ref(vgc_stack* stack);

typedef struct _vgc_str{
  VGCHEADER;
  usize_t str_len;
  union {
    char c[1];
    unsigned char b[1];
  } u;
} vgc_str;

vslot* vgc_str_new(vgc_heap*  heap,
		   vgc_stack* stack,
		   usize_t    str_len,
		   int        area_type);

vslot* vgc_str_newc(vgc_heap*  heap,
		    vgc_stack* stack,
		    char*      charp,
		    usize_t    str_len,
		    int        area_type);

vslot* vgc_str_new_by_buff(vgc_heap*  heap,
			   vgc_stack* stack,
			   ubuffer*   buff,
			   int        area_type);

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

vslot*
vgc_subr_new(vgc_heap*  heap,
	     vgc_stack* stack,
	     usize_t    para_len,
	     usize_t    local_len,
	     vslot*     bc,
	     vslot*     consts);

typedef struct _vcontext{
  VGCHEADER;
  struct _vm* vm;
  vgc_stack* cache;
  /*vgc_stack*/
  vslot stack;
  /*vgc_call*/
  vslot curr_call;
} vcontext;

typedef int (*vcfun_t)(vcontext* ctx);

typedef struct _vgc_cfun{
  VGCHEADER;
  usize_t para_len;
  usize_t local_len;
  vcfun_t entry;
} vgc_cfun;

vslot* vgc_cfun_new(vgc_heap*  heap,
		    vgc_stack* stack,
		    usize_t    para_len,
		    vcfun_t    entry,
		    int        area_type);

typedef struct _vgc_call{
  VGCHEADER;
  unsigned char* pc;
  usize_t base;
  /*vgc_subr*/
  vslot subr;
  /*vgc_cfun*/
  vslot cfun;
  /*vgc_stack*/
  vslot locals;
  /*vgc_call*/
  vslot caller;
} vgc_call;

vslot*
vgc_call_new(vgc_heap*  heap,
	     vgc_stack* stack,
	     usize_t    base,
	     vslot*     slotp_subr,
	     vslot*     slotp_cfun,
	     vslot*     slotp_locals,
	     vslot*     slotp_caller);

typedef struct _vgc_objex_t{
  ustring* type_name;
} vgc_objex_t;

#define VGCHEADEREX \
  VGCHEADER	    \
  vgc_objex_t* oet;

#define VGCTYPEOFEX(objex,type)			\
  ((objex)->oet==(type))

typedef struct _vgc_objex{
  VGCHEADEREX
} vgc_objex;

vslot* _vgc_objex_new(vgc_heap*    heap,
		      vgc_stack*   stack,
		      usize_t      objex_size,
		      usize_t      slot_len,
		      vgc_objex_t* objex_type,
		      int          area_type);

#define vgc_objex_new(heap,stack,stype,slen,otype,atype)	\
  _vgc_objex_new(heap,stack,sizeof(stype),slen,otype,atype)

ustring* vstrtb_put(char* charp,int len);

ustring* vstrtb_put_by_buff(ubuffer* buff);

vgc_objex_t* vgc_objex_init(char* str);

#define vgc_objex_is_init(otype) ((otype) != NULL)

#endif
