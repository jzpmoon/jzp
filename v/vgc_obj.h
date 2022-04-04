#ifndef _VGC_OBJ_H_
#define _VGC_OBJ_H_

#include "udef.h"
#include "ustring_table.h"
#include "ubuffer.h"
#include "ustack_tpl.h"
#include "uhstb_tpl.h"
#include "ulist_tpl.h"
#include "vmacro.h"

enum {
  vgc_obj_type_array,
  vgc_obj_type_string,
  vgc_obj_type_cfun,
  vgc_obj_type_subr,
  vgc_obj_type_call,
  vgc_obj_type_ctx,
  vgc_obj_type_extend,
  vgc_obj_type_ref,
};

#define vgc_obj_typeof(O,T)			\
  (((O)->_mark.t==(T)))

struct mark_t{
  unsigned char m : 1;
  unsigned char a : 1;
  unsigned char t : 6;
};

#define VGCHEADER				\
  struct _vgc_obj* _addr;			\
  usize_t _size;				\
  usize_t _len;					\
  struct mark_t _mark

typedef struct _vgc_obj{
  VGCHEADER;
} vgc_obj,* vgc_objp;

#define vgc_obj_ref_check(obj,index) \
  (index < 0 || index >= (obj)->_len)

#define vgc_obj_log(obj)			\
  do{						\
    ulog("*******vgc_obj_log");			\
    ulog("addr:%p",obj);			\
    ulog("type:%d",obj->_mark.t);		\
    ulog("size:%d",obj->_size);			\
    ulog("len: %d",obj->_len);			\
  } while(0)

enum{
  vslot_type_ref,
  vslot_type_num,
  vslot_type_int,
  vslot_type_char,
  vslot_type_bool,
};

typedef struct _vslot{
  int t;
  union {
    double num;
    vgc_obj* ref;
    int inte;
    int bool;
    int chara;
  } u;
} vslot;

#define vbool_true (1)
#define vbool_false (0)

#define vslot_define_begin			\
  struct{
#define vslot_define(type,name)			\
  vslot name
#define vslot_define_end			\
  } _u;
#define vslot_is_num(slot)			\
  ((slot).t == vslot_type_num)
#define vslot_is_int(slot)			\
  ((slot).t == vslot_type_int)
#define vslot_is_char(slot)			\
  ((slot).t == vslot_type_char)
#define vslot_is_bool(slot)			\
  ((slot).t == vslot_type_bool)
#define vslot_is_ref(slot)			\
  ((slot).t == vslot_type_ref)
#define vslot_is_null(slot)			\
  ((slot).u.ref == NULL)
#define vslot_bool_get(slot)			\
  ((slot).u.bool)
#define vslot_bool_set(slot,val)		\
  (slot.t = vslot_type_bool,slot.u.bool = val)
#define vslot_num_get(slot)			\
  ((slot).u.num)
#define vslot_num_set(slot,val)			\
  (slot.t = vslot_type_num,slot.u.num = val)
#define vslot_int_get(slot)			\
  ((slot).u.inte)
#define vslot_int_set(slot,val)			\
  (slot.t = vslot_type_int,slot.u.inte = val)
#define vslot_char_get(slot)			\
  ((slot).u.chara)
#define vslot_char_set(slot,val)		\
  (slot.t = vslot_type_char,slot.u.chara = val)
#define vslot_ref_get(slot,obj_type)		\
  ((obj_type*)(slot).u.ref)
#define vslot_ref_set(slot,obj)			\
  (slot.t = vslot_type_ref,			\
   slot.u.ref = (vgc_obj*)obj)
#define vslot_null_set(slot)			\
  ((slot).t = vslot_type_ref,			\
   (slot).u.ref = NULL)
#define vslot_is_true(slot)			\
  (vslot_is_bool(slot) && (slot).u.bool)

#define vslot_log(slot)				\
  ulog1("slot type:%d",slot.t);			\
  ulog1("slot num:%f",slot.u.num);		\
  ulog1("slot int:%d",slot.u.inte);		\
  ulog1("slot char:%c",slot.u.chara);		\
  ulog1("slot bool:%d",slot.u.bool);		\
  ulog1("slot ref:%p",slot.u.ref);

vslot vslot_num_eq(vslot slot1,vslot slot2);
vslot vslot_int_eq(vslot slot1,vslot slot2);
vslot vslot_ref_eq(vslot slot1,vslot slot2);

enum{
  vgc_heap_area_static,
  vgc_heap_area_active,
};

ustack_decl_tpl(vslot)
ustack_decl_tpl(vgc_objp)
  
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

vapi vgc_heap* vcall
vgc_heap_new(usize_t area_static_size,
             usize_t area_active_size,
             usize_t root_set_size);

vapi vgc_obj* vcall
vgc_heap_data_new(vgc_heap* heap,
                  usize_t obj_size,
                  usize_t ref_count,
                  int obj_type,
                  int area_type);

#define vgc_obj_slot_count(type)		\
  (sizeof(((type*)0)->_u)/sizeof(vslot))

#define vgc_heap_obj_new(heap,type,obj_type,area_type)			\
  (type*)vgc_heap_data_new						\
  (heap,sizeof(type),vgc_obj_slot_count(type),obj_type,area_type)

#define vgc_slot_get(obj,slot)			\
  ((obj)->_u.slot)

#define vgc_slot_set(obj,slot,slot_val)		\
  ((obj)->_u.slot = slot_val)

#define vgc_obj_slot_list(obj)			\
  (vslot*)((char*)obj + obj->_size -		\
	   (sizeof(vslot) * obj->_len))

#define vgc_obj_null_set(obj,slot)		\
  vslot_null_set((obj)->_u.slot)

#define vgc_obj_ref_get(obj,slot,obj_type)	\
  vslot_ref_get((obj)->_u.slot,obj_type)

#define vgc_obj_ref_set(obj,slot,vobj)		\
  vslot_ref_set((obj)->_u.slot,vobj)

typedef struct _vgc_array{
  VGCHEADER;
  usize_t top;
  vslot objs[1];
} vgc_array;

vapi vgc_array* vcall
vgc_array_new(vgc_heap* heap,
              usize_t array_length,
               int area_type);

vapi int vcall
vgc_array_push(vgc_array* array,vslot slot);

vapi vslot vcall
vgc_array_pop(vgc_array* array);

vapi void vcall
vgc_array_set(vgc_array* array,usize_t idx,vslot slot);

typedef struct _vgc_string{
  VGCHEADER;
  usize_t len;
  union{
    char c[1];
    unsigned char b[1];
  } u;
} vgc_string;

vapi vgc_string* vcall
vgc_string_new(vgc_heap* heap,
			   usize_t string_length,
			   int area_type);

vapi void vcall
vgc_ustr2vstr(vgc_string* vstr,ustring* ustr);

#define vgc_str_bound_check(obj,index) \
  (index >= 0 && index < obj->len)

typedef struct _vgc_ref{
  VGCHEADER;
  vslot_define_begin
  vslot_define(vgc_obj,ref);
  vslot_define_end
} vgc_ref;

vapi vgc_ref* vcall
vgc_ref_new(vgc_heap* heap,vslot slot);

typedef struct _vgc_obj_ex_t{
  char* type_name;
} vgc_objex_t;

#define VGCHEADEREX \
  VGCHEADER;	    \
  vgc_objex_t* oet

typedef struct _vgc_extend{
  VGCHEADEREX;
} vgc_extend;

vapi vgc_extend* vcall
vgc_extend_new(vgc_heap* heap,
			   int struct_size,
			   int ref_count,
			   vgc_objex_t* oet);

#define vgc_objvex_new(heap,stype,oet)  \
  (stype*) vgc_extend_new(heap,sizeof(stype),0,oet)

#define vgc_objfex_new(heap,stype,oet)  \
  (stype*) vgc_extend_new(heap,sizeof(stype),vgc_obj_slot_count(stype),oet)

#endif
