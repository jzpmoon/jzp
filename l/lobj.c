#include <stddef.h>
#include "ustream.h"
#include "lobj.h"

vgc_objex_t* lcons_type;

vslot* lcons_new(vgc_heap* heap,vgc_stack* stack){
  vslot* slotp;
  if(!vgc_objex_is_init(lcons_type)){
    lcons_type = vgc_objex_init("cons");
  }
  slotp = vgc_objex_new(heap,
			stack,
			lcons,
			2,
			lcons_type,
			area_active);
  if(slotp){
    lcons* cons = (lcons*)
      vslot_ref_get(*slotp);
    cons->car = VSLOT_NULL;
    cons->cdr = VSLOT_NULL;
  }
  return slotp;
}

vgc_objex_t* lsymbol_type;

vslot* lsymbol_new(vgc_heap*  heap,
		   vgc_stack* stack,
		   ustring*   key){
  vslot* slotp;
  if(!vgc_objex_is_init(lsymbol_type)){
    lsymbol_type = vgc_objex_init("symbol");
  }
  slotp = vgc_objex_new(heap,
			stack,
			lsymbol,
			1,
			lsymbol_type,
			area_active);
  if(slotp){
    lsymbol* symbol = (lsymbol*)
      vslot_ref_get(*slotp);
    symbol->key = key;
    symbol->val = VSLOT_NULL;
  }
  return slotp;
}

vgc_objex_t* lstream_type;

vslot* lstream_new_by_file(vgc_heap*  heap,
			   vgc_stack* stack,
			   char* file_path){
  URI_DEFINE;
  vslot* slotp;
  FILE* file;
  ustream* stream;
  if(!vgc_objex_is_init(lstream_type)){
    lstream_type = vgc_objex_init("stream");
  }
  slotp = vgc_objex_new(heap,
			stack,
			lstream,
			0,
			lstream_type,
			area_active);
  if(!slotp){
    uabort("lstream new error!");
  }

  file = fopen(file_path,"r");
  if(!file){
    uabort("open file error!");
  }

  stream = ustream_new_by_file(USTREAM_INPUT,file,URI_REF);
  URI_ERROR;
  uabort(URI_DESC);
  URI_END;

  ((lstream*)vslot_ref_get(*slotp))
    ->stream = stream;
  
  return slotp;
}
