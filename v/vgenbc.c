#include "vgenbc.h"

static usize_t
inst_full_length(ulist_vps_instp* insts)
{
  ucursor cursor;
  usize_t length = 0;
  
  insts->iterate(&cursor);
  while (1) {
    vps_instp* inst = insts->next((uset*)insts,&cursor);
    if (!inst) {
      break;
    }
    switch ((*inst)->opc.opcode) {
#define DF(code,name,value,len,oct)			\
      case code:				\
	if (vbytecode_is_vaild(value)) {	\
	  length += len;			\
	}					\
	break;
      VBYTECODE
#undef DF
    }
  };
  return length;
}

static usize_t
inst_byte_length(ulist_vps_instp* insts,usize_t offset)
{
  usize_t i      = 0;
  usize_t length = 0;
  ucursor cursor;
  
  insts->iterate(&cursor);
  while (i < offset) {
    vps_instp* inst = insts->next((uset*)insts,&cursor);
    if (!inst) {
      break;
    }
    switch ((*inst)->opc.opcode) {
#define DF(code,name,value,len,oct)			\
      case code:				\
	if (vbytecode_is_vaild(value)) {	\
	  length += len;			\
	}					\
	break;
      VBYTECODE
#undef DF
    }
    i++;
  }
  return length;
}

vgc_string* vps_inst_to_str(vgc_heap* heap,ulist_vps_instp* insts)
{
  ucursor cursor;
  usize_t byte_count;
  usize_t length;
  vgc_string* str;
  
  byte_count = 0;
  length = inst_full_length(insts);
  str = vgc_string_new(heap,
		       length,
		       vgc_heap_area_static);
  if (!str) {
    return NULL;
  }
  insts->iterate(&cursor);
  while (1) {
    vps_instp* instp = insts->next((uset*)insts,&cursor);
    vps_inst* inst;
    if (!instp) {
      break;
    }
    inst = *instp;
    vps_inst_log(inst);
    switch (inst->opc.opcode) {
#define DF(ocode,name,value,len,oct)				\
      case ocode: {						\
	int i = 0;						\
	usize_t operand;					\
	if (!vbytecode_is_vaild(value)) break;			\
	str->u.b[byte_count++] = ocode;				\
	if (len > 1) {						\
	  if (vps_inst_opck_get(inst) == viopck_branch) {	\
	    operand = vps_inst_imm_get(inst);			\
	    operand = inst_byte_length(insts,			\
				       operand);		\
	  } else {						\
	    operand = vps_inst_imm_get(inst);			\
	  }							\
	  while (i < len - 1) {					\
	    str->u.b[byte_count++] = operand>>(8*i);		\
	    i++;						\
	  }							\
	}							\
	break;							\
      }						
      VBYTECODE				
#undef DF
    }
  }

  return str;
}
