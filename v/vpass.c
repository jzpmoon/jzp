#include "vbyte_code.h"
#include "vpass.h"

vinst*
vinst_new(usize_t code,usize_t operand){
  vinst* inst;
  unew(inst,sizeof(vinst),uabort("vinst:new error!"););
  inst->opcode  = code;
  inst->operand = operand;
  return inst;
}

usize_t
vinst_full_length(ulist* insts){
  ulist*  header = insts;
  ulist*  node   = insts;
  usize_t length = 0;
  do{
    vinst* inst = node->value;
    switch(inst->opcode){
#define DF(code,name,value,len)			\
      case (code):length+=len;break;
      VBYTE_CODE
#undef DF
    }
  } while((node=node->next)!=header);
  return length;
}

usize_t
vinst_byte_length(ulist* insts,usize_t offset){
  usize_t i      = 0;
  usize_t length = 0;
  ulist*  node   = insts;
  while(i < offset){
    vinst* inst = node->value;
    switch(inst->opcode){
#define DF(code,name,value,len)			\
      case (code):length+=len;break;
      VBYTE_CODE
#undef DF
    }
    node = node->next;
    i++;
  }
  return length;
}

int vinst_to_str(vcontext* ctx,ulist* insts){
  ulist*   header     = insts;
  ulist*   node       = insts;
  usize_t  inst_count = 0;
  usize_t  byte_count = 0;
  usize_t  length     = vinst_full_length(insts);
  vgc_string* str;
  str = vgc_string_new(ctx->heap,
		       length,
		       vgc_heap_area_active);
  if(!str){
    uabort("vinst_to_str:vgc_string new error!");
  }
  do{
    vinst* inst=node->value;
    switch(inst->opcode){
#define DF(ocode,name,value,len)					\
      case (ocode):{							\
	int i = 0;							\
	usize_t operand;						\
	if(ocode == Bjmp || ocode == Bjmpi){				\
	  operand = vinst_byte_length(header,				\
				      inst_count + inst->operand);	\
	}else{								\
	  operand = inst->operand;					\
	}								\
	str->u.b[byte_count++] = inst->opcode;				\
	while(i<len-1&&i<sizeof(usize_t)){				\
	  str->u.b[byte_count++] = operand>>(8*i);			\
	  i++;								\
	}								\
	inst_count++;							\
	break;								\
      }						
      VBYTE_CODE				
#undef DF
	}
  }while((node = node->next) != header);
  vgc_push_obj(ctx->heap,str);
  return 0;
}

vdfg_block* vdfg_block_new(){
  vdfg_block* b = ualloc(sizeof(vdfg_block));
  if(b){
    b->t = vdfgk_blk;
    b->link = NULL;
    b->insts = NULL;
  }
  return b;
}

vdfg_graph* vdfg_graph_new(){
  vdfg_graph* g = ualloc(sizeof(vdfg_graph));
  if(g){
    g->t = vdfgk_grp;
    g->link = NULL;
    g->name = NULL;
    g->dfgs = NULL;
    g->entry = NULL;
  }
  return g;
}

vgc_string* vpass_dfg2bin(vps_dfg* dfg){
  return NULL;
}
