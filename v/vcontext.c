#include "vcontext.h"

#define VCONTEXT_OBJTB_SIZE 17
#define VCONTEXT_SYMTB_SIZE 17
#define VCONTEXT_STRTB_SIZE 17
#define VCONTEXT_CONSTS_SIZE 10

vcontext* vcontext_new(vgc_heap* heap){
  vcontext* ctx;
  uhash_table* objtb;
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
  
  objtb = uhash_table_new(VCONTEXT_OBJTB_SIZE);
  if(!objtb){
    uabort("vcontext_new:objtb new error!");
  }
  
  symtb = uhash_table_new(VCONTEXT_SYMTB_SIZE);
  if(!symtb){
    uabort("vcontext_new:symtb new error!");
  }

  strtb = uhash_table_new(VCONTEXT_STRTB_SIZE);
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

static void* vobjtb_key_put(void* key){
  vsymbol* symbol = (void*)key;
  vsymbol* new_symbol = vsymbol_new(symbol->name,symbol->slot);
  if(!new_symbol){
    uabort("vobjtb key put error!");
  }
  return new_symbol;
}

static int vobjtb_key_comp(void* k1,void* k2){
  vsymbol* sym1 = (vsymbol*)k1;
  vsymbol* sym2 = (vsymbol*)k2;
  return (sym1->name - sym2->name);
}

vsymbol* vcontext_obj_put(vcontext* ctx,ustring* name,vslot obj){
  vsymbol symbol = (vsymbol){name,obj};
  vsymbol* new_symbol = 
    uhash_table_put(ctx->objtb,
		    name->hash_code % VCONTEXT_OBJTB_SIZE,
		    (void*)&symbol,
		    vobjtb_key_put,
		    vobjtb_key_comp);
  return new_symbol;
}

int vcontext_load(vcontext* ctx,vps_t* vps){
  
  return 0;
}
