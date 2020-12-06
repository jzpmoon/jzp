#include "uhstb_tpl.c"
#include "ulist_tpl.c"
#include "vbytecode.h"
#include "vm.h"
#include "vpass.h"

uhstb_def_tpl(vps_datap);
uhstb_def_tpl(vdfg_graphp);
ulist_def_tpl(vpsp);
ulist_def_tpl(vps_instp);
ulist_def_tpl(vps_dfgp);
ulist_def_tpl(vinstp);

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
    inst->inst.opcode = opcode;
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

int vinst_to_str(vcontext* ctx,ulist_vinstp* insts){
  ucursor cursor;
  usize_t inst_count = 0;
  usize_t byte_count = 0;
  usize_t length     = vinst_full_length(insts);
  vgc_string* str;
  str = vgc_string_new(ctx->heap,
		       length,
		       vgc_heap_area_active);
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
      VBYTECODE				
#undef DF
	}
  };
  vgc_heap_obj_push(ctx->heap,str);
  return 0;
}

vps_data* vps_num_new(ustring* name,
		      double num,
		      int stk){
  vps_data* data = ualloc(sizeof(vps_data));
  if(data){
    data->t = vpsk_dt;
    data->dtk = vdtk_num;
    data->stk = stk;
    data->name = name;
    data->u.number = num;
  }
  return data;
}

vps_data* vps_any_new(ustring* name,
		      int stk){
  vps_data* data = ualloc(sizeof(vps_data));
  if(data){
    data->t = vpsk_dt;
    data->dtk = vdtk_any;
    data->name = name;
  }
  return data;
}

vdfg_block* vdfg_block_new(){
  vdfg_block* b = ualloc(sizeof(vdfg_block));
  if(b){
    b->t = vdfgk_blk;
    b->parent = NULL;
    b->insts = ulist_vps_instp_new();
  }
  return b;
}

int vdfg_blk2inst(vdfg_block* blk,ulist_vinstp* insts){
  ulist_vps_instp* insts_l1;
  ucursor cursor;
  insts_l1 = blk->insts;
  insts->iterate(&cursor);

  while(1){
    vps_instp* instp = insts->next((uset*)insts_l1,&cursor);
    vps_inst* inst_l1;
    vinst* inst;

    if(!instp){
      break;
    }
    inst_l1 = *instp;
    switch(inst_l1->instk){
    case vinstk_imm:
      inst = &inst_l1->inst;
      if(inst_l1->u.data == NULL){
	uabort("inst imm not find!");
      }
      inst->operand = inst_l1->u.data->idx;
      ulog("inst imm");
      ulist_vinstp_append(insts,inst);
      break;
    case vinstk_locdt:{
      vps_t* dfg = blk->parent;
      vdfg_graph* grp;
      vps_data* data;

      if(!dfg || dfg->t != vdfgk_grp){
	uabort("vdfg_block have no parent!");
      }
      grp = (vdfg_graph*)dfg;
      data = vdfg_grp_dtget(grp,inst_l1->label);
      if(!data){
	uabort1("local variable: %s not find",inst_l1->label->value);
      }
      inst = &inst_l1->inst;
      inst->operand = data->idx;
      ulog("inst data");
      ulist_vinstp_append(insts,inst);
    }
      break;
    case vinstk_code:
      ulog("inst non");
      break;
    case vinstk_non:
      inst = &inst_l1->inst;
      ulog("inst non");
      ulist_vinstp_append(insts,inst);
      break;
    default:
      break;
    }
  }
  return 0;
}

vdfg_graph* vdfg_graph_new(){
  vdfg_graph* g = ualloc(sizeof(vdfg_graph));
  if(g){
    g->t = vdfgk_grp;
    g->parent = NULL;
    g->name = NULL;
    g->locals = uhstb_vps_datap_new(VDFG_GRP_DATA_TABLE_SIZE);
    g->dfgs = ulist_vps_dfgp_new();
    g->entry = NULL;
  }
  return g;
}

static int vdfg_grp_dt_put_comp(vps_datap* data1,vps_datap* data2){
  vps_data* d1;
  vps_data* d2;
  ustring* n1;
  ustring* n2;
  d1 = *data1;
  d2 = *data2;
  n1 = d1->name;
  n2 = d2->name;
  return ustring_comp(n1,n2);
}

void vdfg_grp_dtapd(vdfg_graph* grp,vps_data* dt){
  ustring* name = dt->name;
  unsigned int hscd = 0;
  if(name){
    hscd = name->hash_code;
  }
  uhstb_vps_datap_put(grp->locals,
		      hscd,
		      &dt,
		      NULL,
		      NULL,
		      vdfg_grp_dt_put_comp);
}

static int vdfg_grp_dt_get_comp(vps_datap* data1,vps_datap* data2){
  vps_data* d1;
  vps_data* d2;
  ustring* n1;
  ustring* n2;
  d1 = *data1;
  d2 = *data2;
  n1 = d1->name;
  n2 = d2->name;
  return ustring_comp(n1,n2);
}

vps_data* vdfg_grp_dtget(vdfg_graph* grp,ustring* name){
  vps_data dt_in;
  vps_datap dt_ink = &dt_in;
  vps_datap* dt_outk;

  dt_in.name = name;
  uhstb_vps_datap_get(grp->locals,
		      name->hash_code,
		      &dt_ink,
		      &dt_outk,
		      vdfg_grp_dt_get_comp);
  if(dt_outk){
    return *dt_outk;
  }else{
    return NULL;
  }
}

vps_mod* vps_mod_new(){
  vps_mod* mod = ualloc(sizeof(vps_mod));
  if(mod){
    mod->t = vpsk_mod;
    mod->data = uhstb_vps_datap_new(VPS_MOD_DATA_TABLE_SIZE);
    if(!mod->data){
      uabort("new hash table data error!");
    }
    mod->code = uhstb_vdfg_graphp_new(VPS_MOD_CODE_TABLE_SIZE);
    if(!mod->code){
      uabort("new hash table code error!");
    }
    mod->entry = NULL;
  }
  return mod;
}

static int vps_mod_data_comp(vps_datap* data1,vps_datap* data2){
  vps_data* d1 = *data1;
  vps_data* d2 = *data2;
  if(d1 > d2){
    return 1;
  }else if(d1 < d2){
    return -1;
  }else{
    return 0;
  }
}

static int vps_mod_code_comp(vdfg_graphp* data1,vdfg_graphp* data2){
  vdfg_graph* g1 = *data1;
  vdfg_graph* g2 = *data2;
  if(g1 > g2){
    return 1;
  }else if(g1 < g2){
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
  uhstb_vps_datap_put(mod->data,
		      hscd,
		      &data,
		      NULL,
		      NULL,
		      vps_mod_data_comp);
}

void vps_mod_code_put(vps_mod* mod,vdfg_graph* code){
  ustring* name = code->name;
  unsigned int hscd = 0;
  if(name){
    hscd = name->hash_code;
  }
  uhstb_vdfg_graphp_put(mod->code,
			hscd,
			&code,
			NULL,
			NULL,
			vps_mod_code_comp);
}

vgc_string* vpass_dfg2bin(vps_dfg* dfg){
  return NULL;
}
