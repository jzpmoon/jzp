#include "uhstb_tpl.c"
#include "vcontext.h"

uhstb_def_tpl(vsymbol);

#define VCONTEXT_OBJTB_SIZE 17
#define VCONTEXT_SYMTB_SIZE 17
#define VCONTEXT_STRTB_SIZE 17
#define VCONTEXT_CONSTS_SIZE 10

vcontext* vcontext_new(vgc_heap* heap){
  vcontext* ctx;
  uhstb_vsymbol* objtb;
  ustring_table* symtb;
  ustring_table* strtb;
  vgc_array* consts;

  ctx = vgc_heap_obj_new(heap,
			 vcontext,
			 vgc_obj_type_ctx,
			 vgc_heap_area_static);

  if(!ctx){
    uabort("vcontext_new: ctx new error!");
  }
  
  objtb = uhstb_vsymbol_new(VCONTEXT_OBJTB_SIZE);
  if(!objtb){
    uabort("vcontext_new:objtb new error!");
  }
  
  symtb = ustring_table_new(VCONTEXT_SYMTB_SIZE);
  if(!symtb){
    uabort("vcontext_new:symtb new error!");
  }

  strtb = ustring_table_new(VCONTEXT_STRTB_SIZE);
  if(!strtb){
    uabort("vcontext_new:strtb new error!");
  }
  consts = vgc_array_new(heap,
			 VCONTEXT_CONSTS_SIZE,
			 vgc_heap_area_static);
  if(!consts){
    uabort("vcontext_new:consts new error!");
  }

  ctx->heap = heap;
  ctx->objtb = objtb;
  ctx->symtb = symtb;
  ctx->strtb = strtb;
  vgc_obj_null_set(ctx,calling);
  vgc_obj_ref_set(ctx,consts,consts);
  return ctx;
}

vsymbol* vsymbol_new(ustring* name,vslot slot){
  vsymbol* symbol = ualloc(sizeof(vsymbol));
  if(symbol){
    symbol->name = name;
    symbol->slot = slot;
  }
  return symbol;
}

static int vobjtb_key_comp(vsymbol* sym1,vsymbol* sym2){
  return (sym1->name - sym2->name);
}

vsymbol* vcontext_obj_put(vcontext* ctx,ustring* name,vslot obj){
  vsymbol symbol = (vsymbol){name,obj};
  vsymbol* new_symbol;
  int retval = uhstb_vsymbol_put(ctx->objtb,
				 name->hash_code,
				 &symbol,
				 &new_symbol,
				 NULL,
				 vobjtb_key_comp);
  if(retval == -1){
    uabort("vcontext_obj_put error!");
  }
  return new_symbol;
}

int vcontext_load(vcontext* ctx,vps_t* vps){
  switch(vps->t){
  case vpsk_dt:{
    vps_data* data = (vps_data*)vps;
    vgc_array* consts = vgc_obj_ref_get(ctx,consts,vgc_array);
    vslot slot;
    int top;
    ulog("vcontext_load data");
    if(data->dtk == vdtk_num){
      vslot_num_set(slot,data->u.number);      
    }else if(data->dtk == vdtk_int){
      vslot_int_set(slot,data->u.integer);
    }else if(data->dtk == vdtk_arr){
      break;
    }else if(data->dtk == vdtk_any){
      vslot_null_set(slot);
    }else{
      uabort("vcontext_load unknow data type");
    }
    top = vgc_array_push(consts,slot);
    if(top == -1){
      uabort("vcontext_load consts overflow!");
    }
    data->idx = top;
    break;
  }
  case vpsk_mod:{
    vps_mod* mod = (vps_mod*)vps;
    ucursor cursor;
    uhstb_vps_datap* data = mod->data;
    uhstb_vdfg_graphp* code = mod->code;
    ulog("vcontext_load mod");
    (data->iterate)(&cursor);
    ulog("vcontext_load mod data");
    while(1){
      vps_datap* dp = (data->next)((uset*)data,&cursor);
      if(!dp){
	break;
      }
      ulog("vcontext_load mod data entry");
      vcontext_load(ctx,(vps_t*)*dp);
    }
    ulog("vcontext_load mod code");
    (data->iterate)(&cursor);
    while(1){
      vdfg_graphp* gp = (code->next)((uset*)code,&cursor);
      if(!gp){
	break;
      }
      ulog("vcontext_load mod graph entry");
      vcontext_load(ctx,(vps_t*)*gp);
    }
    break;
  }
  case vdfgk_grp:{
    ulog("vcontext_load graph");
    break;
  }
  case vdfgk_blk:{
    ulog("vcontext_load block");
    break;
  }
  default:
    uabort("vcontext_load unknow vps type!");
  }
  return 0;
}
