#include <stddef.h>
#include "lobj.h"

void lobj_stream_new_by_file(vgc_heap* heap,
			     char* file_path){
  URI_DEFINE;
  vgc_objex_t stream_type = {"stream"};
  lobj_stream* lstream;
  FILE* file;
  ustream* stream;
  lstream = vgc_objex_new(heap,lobj_stream,stream_type);
  if(!lstream){
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

  lstream->stream = stream;
  vgc_heap_obj_push(heap,lstream);
}
