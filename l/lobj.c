#include <stddef.h>
#include "lobj.h"

vslot* lobj_cons_new(vgc_heap* heap){
  static vgc_obj_ex_t cons_t = {"cons"};
  vslot* slotp;
  slotp = vgc_extend_new(heap,cons_type);
  if(slotp){
    lobj_cons* cons = vslot_ref_get(*slotp,lobj_cons);
    vslot_null_set(cons->car);
    vslot_null_set(cons->cdr);
  }
  return slotp;
}

vslot* lobj_symbol_new(vgc_heap* heap,
		       ustring* key){
  static vgc_obj_ex_t symbol_type = {"symbol"};
  vslot* slotp;
  slotp = vgc_extend_new(heap,symbol_type);
  if(slotp){
    lobj_symbol* symbol = vslot_ref_get(*slotp,lobj_symbol);
    symbol->key = key;
  }
  return slotp;
}

vslot* lobj_stream_new_by_file(vgc_heap* heap,
			       char* file_path){
  URI_DEFINE;
  static vgc_obj_ex_t stream_type = {"stream"};
  vslot* slotp;
  FILE* file;
  ustream* stream;
  slotp = vgc_extend_new(heap,stream_type);
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

  (vslot_ref_get(*slotp,lobj_stream))->stream = stream;
  
  return slotp;
}

vslot* lobj_parser_new(vgc_heap* heap){
  static vgc_obj_ex_t parser_type = {"parser"};
  vslot* slotp;
  ltoken_state* ts;

  ts = ltoken_state_new(NULL);
  if(!ts){
    return NULL;
  }
  
  slotp = vgc_extend_new(heap,parser_type);
  if(slotp){
    lobj_parser* parser = vslot_ref_get(*slotp,lobj_parser);
    parser->ts = ts;
  }
  return slotp;
}
