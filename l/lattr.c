#include "lparser.h"

void* last_attr_key_put(void* key){
  return key;
}

void* last_attr_key_get(void* key){
  return key;
}

int last_attr_put_comp(void* k1,void* k2){
  last_attr* attr1 = (last_attr*)k1;
  last_attr* attr2 = (last_attr*)k2;
  return ustring_comp(attr1->name,attr2->name);
}

int last_attr_get_comp(void* k1,void* k2){
  ustring* sym = (ustring*)k1;
  last_attr* attr = (last_attr*)k2;
  return ustring_comp(sym,attr->name);
}

LDEFATTR(subr,"subr",{
    last_obj* next;
    last_obj* obj;
    last_symbol* subr_name;
    last_cons* args;
    vdfg_graph* dfg;
    
    next = last_cdr(ast_obj);
    if(next->t != lastk_cons){
      uabort("subr error!");
    }
    obj = last_car(next);
    if(obj->t != lastk_symbol){
      uabort("subr name invalid!");
    }
    subr_name = (last_symbol*)obj;

    next = last_cdr(next);
    if(next->t != lastk_cons){
      uabort("subr error!");
    }
    obj = last_car(next);
    if(obj->t != lastk_cons){
      uabort("subr args invalid!");
    }
    args = (last_cons*)obj;
    
    ulog1("attr subr:%s",subr_name->name->value);
    dfg = vdfg_graph_new();
    if(!dfg){
      uabort("subr new dfg error!");
    }
    dfg->name = subr_name->name;
    
    next = last_cdr(next);
    while(next){
      last_symbol* inst_name;
      if(next->t != lastk_cons){
	uabort("subr error!");
      }
      obj = last_car(next);
      if(obj->t != lastk_cons){
	uabort("subr error!");
      }
      obj = last_car(obj);
      if(obj->t != lastk_symbol){
	uabort("subr inst invalid!");
      }
      inst_name = (last_symbol*)obj;
      if(inst_name->attr){
	last_attr* attr = inst_name->attr;
      }
      ulog1("inst: %s",inst_name->name->value);
      next = last_cdr(next);
    }
  })

#define DF(ocode,name,value,len)		\
  LDEFATTR(ocode,name,{				\
      vdfg_block* block = vdfg_block_new();	\
      if(!block) {				\
	uabort("dfg block new error!");		\
      }						\
    })						\

void ltoken_state_attr_init(ltoken_state* ts){
  LATTR_INIT(ts,subr);
}
