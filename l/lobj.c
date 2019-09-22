#include <stddef.h>
#include "lobj.h"


lcons* lcons_new(vgc_heap* heap){
  static vgc_objex_t lcons_type;
  lcons* cons;
  if(!vgc_objex_is_init(lcons_type)){
    lcons_type = vgc_objex_init("cons");
  }
  cons = vgc_objex_new(heap,lcons,2,lcons_type,area_active);
  if(cons){
    cons->car = NULL;
    cons->cdr = NULL;
  }
  return cons;
}
