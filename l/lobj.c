#include <stddef.h>
#include "lobj.h"

lcons* lcons_new(vgc_heap* heap){
  static vgc_objex_t* lcons_type;
  lcons* cons;
  if(!vgc_objex_is_init(lcons_type)){
    lcons_type = vgc_objex_init("cons");
  }
  cons = vgc_objex_new(heap,lcons,2,lcons_type,area_active);
  if(cons){
    cons->car = VSLOT_NULL;
    cons->cdr = VSLOT_NULL;
  }
  return cons;
}

lsymbol* lsymbol_new(vgc_heap* heap,ustring* key){
  static vgc_objex_t* lsymbol_type;
  lsymbol* symbol;
  if(!vgc_objex_is_init(lsymbol_type)){
    lsymbol_type = vgc_objex_init("symbol");
  }
  symbol = vgc_objex_new(heap,lsymbol,1,lsymbol_type,area_active);
  if(symbol){
    symbol->key = key;
    symbol->val = VSLOT_NULL;
  }
  return symbol;
}
