#ifndef _VGC_OBJ_H_
#define _VGC_OBJ_H_

#include "udef.h"
#include "ustack_tpl.h"
#include "ustring_table.h"
#include "ubuffer.h"

enum {
  vgc_obj_type_stack,
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

#define vslot_is_ref(slot) ((slot).t == vslot_type_ref)
#define vslot_ref_get(slot) ((slot).u.ref)

enum{
  vgc_heap_area_static,
  vgc_heap_area_active,
};

ustack_tpl(vgc_objp)
ustack_tpl(vslot)

typedef struct _vgc_heap{
  usize_t heap_size;
  ustack_vslot root_set;
  ustack_vgc_objp stack;
  usize_t area_static_size;
  vgc_obj* area_static_begin;
  vgc_obj* area_static_index;
  usize_t area_active_size;
  vgc_obj* area_active_begin;
  vgc_obj* area_active_index;
  char heap_data[1];
} vgc_heap;

vgc_heap* vgc_heap_new(usize_t area_static_size,
		       usize_t area_active_size,
		       usize_t root_set_size);

int vgc_heap_data_new(vgc_heap* heap,
		      usize_t obj_size,
		      usize_t ref_count,
		      int area_type);

#define vgc_obj_ref_count(type) \
  (sizeof(((type*)0)->_u)/sizeof(vgc_obj*))

#define vgc_heap_obj_new(heap,type,area_type) \
  vgc_heap_data_new(heap,sizeof(type),vgc_obj_ref_count(type),area_type)

typedef struct _vgc_stack{
  VGCHEADER;
  vgc_obj* objs[1];
} vgc_stack;

int vgc_stack_new(vgc_heap* heap,
		  usize_t stack_size);

#endif
