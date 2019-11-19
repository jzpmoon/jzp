#ifndef _VGC_OBJ_H_
#define _VGC_OBJ_H_

#include "udef.h"
#include "ustack_tpl.h"
#include "ustring_table.h"
#include "ubuffer.h"

enum {
  vgc_obj_type_array,
  vgc_obj_type_string,
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

#define vgc_obj_log(obj)			\
  do{						\
    ulog("*******vgc_obj_log");			\
    ulog1("type:%d",obj->_mark.t);		\
    ulog1("size:%d",obj->_size);		\
    ulog1("len: %d",obj->_len);			\
    ulog1("top: %d",obj->_top);			\
  } while(0)

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
ustack_log_decl_tpl(vgc_objp)
ustack_push_decl_tpl(vgc_objp)
ustack_pop_decl_tpl(vgc_objp)

ustack_tpl(vslot)
ustack_log_decl_tpl(vslot)
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
  (heap,sizeof(type),vgc_obj_ref_count(type),obj_type,area_type)

#define vgc_heap_stack_push(heap,slot)			\
  if(ustack_push_vslot(&(heap)->root_set,slot)){	\
    uabort("vgc_heap_stack: overflow!");		\
  }

#define vgc_heap_stack_pop(heap,slotp)			\
  if(ustack_pop_vslot(&(heap)->root_set,slotp)){	\
    uabort("vgc_heap_stack: empty!");			\
  }

#define vgc_pop_obj(heap,obj,obj_type)		\
  do{						\
    vslot __slot;				\
    vgc_heap_stack_pop(heap,&__slot);		\
    obj = (obj_type*)vslot_ref_get(__slot);	\
  } while(0)

#define vgc_obj_slot_get(heap,obj,slot)		\
  vgc_heap_stack_push(heap,(obj)->_u.slot);
  
#define vgc_obj_slot_set(heap,obj,slot)		\
  vgc_heap_stack_pop(heap,&((obj)->_u.slot));

typedef struct _vcontext{
  vgc_heap* heap;
  struct _vgc_call* calling;
} vcontext;

typedef struct _vgc_array{
  VGCHEADER;
  vgc_obj* objs[1];
} vgc_array;

int vgc_array_new(vgc_heap* heap,
		  usize_t len,
		  int area_type);

typedef struct _vgc_string{
  VGCHEADER;
  union{
    char c[1];
    unsigned char b[1];
  } u;
} vgc_string;

int vgc_string_new(vgc_heap* heap,
		   usize_t len,
		   int area_type);

typedef int(*vcfun_ft)(vcontext*);

typedef struct _vgc_cfun{
  VGCHEADER;
  vcfun_ft entry;
  vslot_define_begin
  vslot_define_end
} vgc_cfun;

int vgc_cfun_new(vgc_heap* heap,
		 vcfun_ft entry,
		 int area_type);

typedef struct _vgc_subr{
  VGCHEADER;
  usize_t params_count;
  usize_t locals_count;
  vslot_define_begin
  vslot_define(consts)
  vslot_define(bytecode)
  vslot_define_end
} vgc_subr;

int vgc_subr_new(vgc_heap* heap,
		 usize_t params_count,
		 usize_t locals_count,
		 int area_type);

enum {
  vgc_call_type_cfun,
  vgc_call_type_subr,
};

typedef struct _vgc_call{
  VGCHEADER;
  int call_type;
  usize_t base;
  char* pc;
  vslot_define_begin
  vslot_define(cfun)
  vslot_define(subr)
  vslot_define(caller)
  vslot_define_end
} vgc_call;

int vgc_call_new(vgc_heap* heap,
		 int call_type,
		 usize_t base);

#endif
