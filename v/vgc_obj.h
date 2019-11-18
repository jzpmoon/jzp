#ifndef _VGC_OBJ_H_
#define _VGC_OBJ_H_

#include "udef.h"
#include "ustack_tpl.h"
#include "ustring_table.h"
#include "ubuffer.h"

enum {
  vgc_obj_type_array,
  vgc_obj_type_str,
  vgc_obj_type_cfun,
  vgc_obj_type_subr,
  vgc_obj_type_call,
  vgc_obj_type_extend,
};

#define vgc_obj_typeof(O,T) \
  (((O)->mark.t==(T)))

struct mark_t{
  unsigned char m : 1;
  unsigned char a : 1;
  unsigned char t : 6;
};

#define VGCHEADER				\
  struct _vgc_obj* _addr;			\
  usize_t _size;				\
  usize_t _len;					\
  usize_t _top;					\
  struct mark_t _mark

typedef struct _vgc_obj{
  VGCHEADER;
} vgc_obj,* vgc_objp;

enum{
  vslot_type_ref,
  vslot_type_num,
  vslot_type_bool,
};

typedef struct _vslot{
  int t;
  union {
    double   num;
    int      bool;
    vgc_obj* ref;
  } u;
} vslot;

#define vslot_define_begin struct{
#define vslot_define(name) vslot name;
#define vslot_define_end } _u;
#define vslot_is_ref(slot) ((slot).t == vslot_type_ref)
#define vslot_ref_get(slot) ((slot).u.ref)
#define vslot_ref_set(slot,obj) \
  (slot.t = vslot_type_ref,slot.u.ref = obj)

enum{
  vgc_heap_area_static,
  vgc_heap_area_active,
};

ustack_tpl(vgc_objp)
ustack_push_decl_tpl(vgc_objp)
ustack_pop_decl_tpl(vgc_objp)

ustack_tpl(vslot)
ustack_push_decl_tpl(vslot)
ustack_pop_decl_tpl(vslot)

typedef struct _vgc_heap_area{
  vgc_obj* area_begin;
  vgc_obj* area_index;
  usize_t area_size;
} vgc_heap_area;
  
typedef struct _vgc_heap{
  usize_t heap_size;
  ustack_vslot root_set;
  ustack_vgc_objp stack;
  vgc_heap_area area_static;
  vgc_heap_area area_active;
  char heap_data[1];
} vgc_heap;

vgc_heap* vgc_heap_new(usize_t area_static_size,
		       usize_t area_active_size,
		       usize_t root_set_size);

int vgc_heap_data_new(vgc_heap* heap,
		      usize_t obj_size,
		      usize_t ref_count,
		      int obj_type,
		      int area_type);

#define vgc_obj_ref_count(type) \
  (sizeof(((type*)0)->_u)/sizeof(vgc_obj*))

#define vgc_heap_obj_new(heap,type,obj_type,area_type)		\
  vgc_heap_data_new						\
  (heap,sizeof(type),vgc_obj_ref_count(type),obj_typearea_type)

#define vgc_heap_stack_push(heap,slot)			\
  if(ustack_push_vslot(&(heap)->root_set,slot)){	\
    uabort("vgc_heap_stack: overflow!");		\
  }

#define vgc_heap_stack_pop(heap,slotp)			\
  if(ustack_pop_vslot(&(heap)->root_set,slotp)){	\
    uabort("vgc_heap_stack: empty!");			\
  }

typedef struct _vgc_array{
  VGCHEADER;
  vgc_obj* objs[1];
} vgc_array;

int vgc_array_new(vgc_heap* heap,
		  usize_t len,
		  int area_type);

#endif
