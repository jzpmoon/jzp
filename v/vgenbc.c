#include "ulist_tpl.c"
#include "vgenbc.h"

ulist_def_tpl(vinstp);

usize_t
vinst_full_length(ulist_vinstp* insts){
  ucursor cursor;
  usize_t length = 0;
  insts->iterate(&cursor);
  while(1){
    vinstp* inst = insts->next((uset*)insts,&cursor);
    if(!inst){
      break;
    }
    switch((*inst)->opcode){
#define DF(code,name,value,len)			\
      case (code):length+=len;break;
      VBYTECODE
#undef DF
    }
  };
  return length;
}

usize_t
vinst_byte_length(ulist_vinstp* insts,usize_t offset){
  usize_t i      = 0;
  usize_t length = 0;
  ucursor cursor;
  insts->iterate(&cursor);
  while(i < offset){
    vinstp* inst = insts->next((uset*)insts,&cursor);
    if(!inst){
      break;
    }
    switch((*inst)->opcode){
#define DF(code,name,value,len)			\
      case (code):length+=len;break;
      VBYTECODE
#undef DF
    }
    i++;
  }
  return length;
}

vgc_string* vinst_to_str(vgc_heap* heap,ulist_vinstp* insts){
  ucursor cursor;
  usize_t byte_count = 0;
  usize_t length = vinst_full_length(insts);
  vgc_string* str;
  str = vgc_string_new(heap,
		       length,
		       vgc_heap_area_static);
  if(!str){
    uabort("vinst_to_str:vgc_string new error!");
  }
  insts->iterate(&cursor);
  while(1){
    vinstp* instp = insts->next((uset*)insts,&cursor);
    vinst* inst;
    if(!instp){
      break;
    }
    inst = *instp;
    switch(inst->opcode){
#define DF(ocode,name,value,len)					\
      case (ocode):{							\
	int i = 0;							\
	usize_t operand;						\
	if(ocode == Bjmp || ocode == Bjmpi){				\
	  operand = vinst_byte_length(insts,				\
				      inst->operand);			\
	}else{								\
	  operand = inst->operand;					\
	}								\
	str->u.b[byte_count++] = inst->opcode;				\
	while(i<len-1&&i<sizeof(usize_t)){				\
	  str->u.b[byte_count++] = operand>>(8*i);			\
	  i++;								\
	}								\
	break;								\
      }						
      VBYTECODE				
#undef DF
	}
  };

  return str;
}
