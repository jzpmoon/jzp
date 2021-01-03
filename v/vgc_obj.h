#ifndef _VGC_OBJ_H_
#define _VGC_OBJ_H_

#include "udef.h"
#include "ustring_table.h"
#include "ubuffer.h"
#include "ustack_tpl.h"
#include "uhstb_tpl.h"
#include "ulist_tpl.h"

enum {
  vgc_obj_type_array,
  vgc_obj_type_string,
  vgc_obj_type_cfun,
  vgc_obj_type_subr,
  vgc_obj_type_call,
  vgc_obj_type_ctx,
  vgc_obj_type_extend,
};

#define vgc_obj_typeof(O,T) \
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
    ulog1("type:%d",obj->_mark.t);		\
    ulog1("size:%d",obj->_size);		\
    ulog1("len: %d",obj->_len);			\
  } while(0)

enum{
  vslot_type_ref,
  vslot_type_num,
  vslot_type_int,
  vslot_type_bool,
  vslot_type_null,
};

typedef struct _vslot{
  int t;
  union {
    double   num;
    int      inte;
    int      bool;
    vgc_obj* ref;
  } u;
} vslot;

#define vbool_true (1)
#define vbool_false (0)

#define vslot_define_begin			\
  struct{
#define vslot_define(type,name)			\
  vslot name;
#define vslot_define_end			\
  } _u;
#define vslot_is_num(slot)			\
  ((slot).t == vslot_type_num)
#define vslot_is_bool(slot)			\
  ((slot).t == vslot_type_bool)
#define vslot_is_ref(slot)			\
  ((slot).t == vslot_type_ref)
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
#define vslot_ref_get(slot,obj_type)		\
  ((obj_type*)(slot).u.ref)
#define vslot_ref_set(slot,obj)			\
  (slot.t = vslot_type_ref,			\
   slot.u.ref = (vgc_obj*)obj)
#define vslot_null_set(slot)			\
  ((slot).t = vslot_type_null,			\
   (slot).u.ref = NULL)
#define vslot_is_true(SLOT)			\
  (vslot_is_bool(SLOT) && (SLOT).u.bool)

#define vslot_log(slot)				\
  ulog1("slot type:%d",slot.t);			\
  ulog1("slot num:%f",slot.u.num);		\
  ulog1("slot bool:%d",slot.u.bool);		\
  ulog1("slot ref:%lld",(long long)slot.u.ref);

vslot vslot_num_add(vslot slot1,vslot slot2);
vslot vslot_num_eq(vslot slot1,vslot slot2);
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

vgc_heap* vgc_heap_new(usize_t area_static_size,
		       usize_t area_active_size,
		       usize_t root_set_size);

vgc_obj* vgc_heap_data_new(vgc_heap* heap,
			   usize_t obj_size,
			   usize_t ref_count,
			   int obj_type,
			   int area_type);

#define vgc_obj_slot_count(type)		\
  (sizeof(((type*)0)->_u)/sizeof(vslot))

#define vgc_heap_obj_new(heap,type,obj_type,area_type)			\
  (type*)vgc_heap_data_new						\
  (heap,sizeof(type),vgc_obj_slot_count(type),obj_type,area_type)

#define vgc_heap_stack_push(heap,slot)			\
  if(ustack_vslot_push(&(heap)->root_set,slot)){	\
    uabort("vgc_heap_stack: overflow!");		\
  }

#define vgc_heap_stack_pop(heap,slotp)			\
  if(ustack_vslot_pop(&(heap)->root_set,slotp)){	\
    uabort("vgc_heap_stack: empty!");			\
  }

vslot vgc_heap_stack_get(vgc_heap* heap,usize_t index);
void vgc_heap_stack_set(vgc_heap* heap,usize_t index,vslot slot);
usize_t vgc_heap_stack_top_get(vgc_heap* heap);
void vgc_heap_stack_top_set(vgc_heap* heap,usize_t index);

#define vgc_heap_stack_pushv(heap)		\
  if(ustack_vslot_pushv(&(heap)->root_set)){	\
    uabort("vgc_heap_stack: overflow!");	\
  }

#define vgc_heap_obj_push(heap,obj)		\
  do{						\
    vslot __slot;				\
    vslot_ref_set(__slot,obj);			\
    vgc_heap_stack_push(heap,__slot);		\
  } while(0)

#define vgc_heap_obj_pop(heap,obj,obj_type)	\
  do{						\
    vslot __slot;				\
    vgc_heap_stack_pop(heap,&__slot);		\
    obj = vslot_ref_get(__slot,obj_type);	\
  } while(0)

#define vgc_obj_slot_get(heap,obj,slot)		\
  vgc_heap_stack_push(heap,(obj)->_u.slot);
  
#define vgc_obj_slot_set(heap,obj,slot)		\
  vgc_heap_stack_pop(heap,&((obj)->_u.slot));

#define vgc_obj_slot_list(obj)			\
  (vslot*)((char*)obj + obj->_size -		\
	   (sizeof(vslot) * obj->_len))

#define vgc_obj_null_set(obj,slot)		\
  vslot_null_set((obj)->_u.slot)

#define vgc_obj_ref_get(obj,slot,obj_type)	\
  vslot_ref_get((obj)->_u.slot,obj_type)

#define vgc_obj_ref_set(obj,slot,vobj)		\
  vslot_ref_set((obj)->_u.slot,vobj)

typedef struct _vsymbol{
  ustring* name;
  vslot slot;
} vsymbol;

typedef struct _vgc_array{
  VGCHEADER;
  int top;
  vslot objs[1];
} vgc_array;

vgc_array* vgc_array_new(vgc_heap* heap,
			 usize_t array_length,
			 int area_type);

int vgc_array_push(vgc_array* array,vslot slot);

vslot vgc_array_pop(vgc_array* array);

void vgc_array_set(vgc_array* array,int idx,vslot slot);

typedef struct _vreloc{
  ustring* ref_name;
  vgc_array* rel_obj;
  int rel_idx;
} vreloc;

#define vreloc_log(reloc)				\
  ulog1("ref_name:%s",reloc->ref_name->value);		\
  ulog1("rel_obj:%lld",(long long)reloc->rel_obj);	\
  ulog1("rel_idx:%d",reloc->rel_idx);

ulist_decl_tpl(vreloc);
uhstb_decl_tpl(vsymbol);

#define VCONTEXT_SYMTB_SIZE 17
#define VCONTEXT_STRTB_SIZE 17

typedef struct _vmod{
  ulist_vreloc* rells;
  uhstb_vsymbol* gobjtb;
  uhstb_vsymbol* lobjtb;
} vmod;

ulist_decl_tpl(vmod);

typedef struct _vcontext{
  VGCHEADER;
  vgc_heap* heap;
  ulist_vmod* mods;
  ustring_table* symtb;
  ustring_table* strtb;
  vslot_define_begin
    vslot_define(vgc_call,calling);
    vslot_define(vgc_array,consts);
  vslot_define_end
} vcontext;

typedef struct _vgc_string{
  VGCHEADER;
  int len;
  union{
    char c[1];
    unsigned char b[1];
  } u;
} vgc_string;

vgc_string* vgc_string_new(vgc_heap* heap,
			   usize_t string_length,
			   int area_type);

void vgc_string_log(vgc_heap* heap);

#define vgc_str_bound_check(obj,index) \
  (index >= 0 && index < obj->len)

typedef int(*vcfun_ft)(vcontext*);

typedef struct _vgc_cfun{
  VGCHEADER;
  vcfun_ft entry;
  int params_count;
  int has_retval;
  vslot_define_begin
  /*
   *void member
   */
  vslot_define_end
} vgc_cfun;

vgc_cfun* vgc_cfun_new(vgc_heap* heap,
		       vcfun_ft entry,
		       int params_count,
		       int has_retval,
		       int area_type);

typedef struct _vgc_subr{
  VGCHEADER;
  usize_t params_count;
  usize_t locals_count;
  vslot_define_begin
    vslot_define(vgc_array,consts);
    vslot_define(vgc_string,bytecode);
  vslot_define_end
} vgc_subr;

vgc_subr* vgc_subr_new(vgc_heap* heap,
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
  unsigned char* pc;
  vslot_define_begin
    vslot_define(vgc_subr,cfun);
    vslot_define(vgc_subr,subr);
    vslot_define(vgc_call,caller);
  vslot_define_end
} vgc_call;

vgc_call* vgc_call_new(vgc_heap* heap,
		       int call_type,
		       usize_t base);

#define vgc_call_is_cfun(call)				\
  ((call)->call_type == vgc_call_type_cfun)

typedef struct _vgc_obj_ex_t{
  char* type_name;
} vgc_objex_t;

#define VGCHEADEREX \
  VGCHEADER;	    \
  vgc_objex_t* oet

typedef struct _vgc_extend{
  VGCHEADEREX;
  vslot_define_begin
  /*
   * void member
   */
  vslot_define_end
} vgc_extend;

vgc_extend* vgc_extend_new(vgc_heap* heap,
			   int struct_size,
			   int ref_count,
			   vgc_objex_t* oet);

#define vgc_objex_new(heap,stype,oet)					\
  (stype*) vgc_extend_new(heap,sizeof(stype),vgc_obj_slot_count(stype),oet);


#endif
