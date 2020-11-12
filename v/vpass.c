#include "vbyte_code.h"
#include "vpass.h"

vps_inst*
vps_inst_new(int instk,
	     usize_t opcode,
	     ustring* label,
	     vps_data* data,
	     vps_dfg* code){
  vps_inst* inst = ualloc(sizeof(vps_inst));
  if(inst){
    inst->t = vpsk_inst;
    inst->instk = instk;
    inst->opcode = opcode;
    inst->label = label;
    if(data){
      inst->u.data = data;
    }else{
      inst->u.code = code;
    }
  }
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
  vgc_heap_obj_push(ctx->heap,str);
  return 0;
}

vps_data* vps_num_new(ustring* name,
		      double num){
  vps_data* data = ualloc(sizeof(vps_data));
  if(data){
    data->dtk = vdtk_num;
    data->name = name;
    data->u.number = num;
  }
  return data;
}

vps_data* vps_any_new(ustring* name){
  vps_data* data = ualloc(sizeof(vps_data));
  if(data){
    data->dtk = vdtk_any;
    data->name = name;
  }
  return data;
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
    g->dts = NULL;
    g->dfgs = NULL;
    g->entry = NULL;
  }
  return g;
}

vps_mod* vps_mod_new(){
  vps_mod* mod = ualloc(sizeof(vps_mod));
  if(mod){
    mod->data = uhash_table_new(VPS_MOD_DATA_TABLE_SIZE);
    if(!mod->data){
      uabort("new hash table data error!");
    }
    mod->code = uhash_table_new(VPS_MOD_CODE_TABLE_SIZE);
    if(!mod->code){
      uabort("new hash table code error!");
    }
  }
  return mod;
}

static void* vps_mod_key_put(void* data){
  return data;
}

static int vps_mod_comp(void* data1,void* data2){
  if(data1 > data2){
    return 1;
  }else if(data1 < data2){
    return -1;
  }else{
    return 0;
  }
}

void vps_mod_data_put(vps_mod* mod,vps_data* data){
  ustring* name = data->name;
  unsigned int hscd = 0;
  if(name){
    hscd = name->hash_code;
  }
  uhash_table_put(mod->data,
		  hscd,
		  data,
		  vps_mod_key_put,
		  vps_mod_comp);
}

void vps_mod_code_put(vps_mod* mod,vdfg_graph* code){
  ustring* name = code->name;
  unsigned int hscd = 0;
  if(name){
    hscd = name->hash_code;
  }
  uhash_table_put(mod->data,
		  hscd,
		  code,
		  vps_mod_key_put,
		  vps_mod_comp);
}

vgc_string* vpass_dfg2bin(vps_dfg* dfg){
  return NULL;
}
