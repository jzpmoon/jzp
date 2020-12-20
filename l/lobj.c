#include <stddef.h>
#include "lobj.h"

vgc_objex_t lobj_stream_type = {"stream"};

lobj_stream* lobj_istream_new_by_file(vgc_heap* heap,
				      FILE* file){
  URI_DEFINE;
  lobj_stream* lstream;
  ustream* stream;
  lstream = vgc_objex_new(heap,lobj_stream,&lobj_stream_type);
  if(!lstream){
    uabort("lstream new error!");
  }

  stream = ustream_new_by_file(USTREAM_INPUT,file,URI_REF);
  URI_ERROR;
    uabort(URI_DESC);
  URI_END;

  lstream->stream = stream;
  return lstream;
}

lobj_stream* lobj_ostream_new_by_file(vgc_heap* heap,
				      FILE* file){
  URI_DEFINE;
  lobj_stream* lstream;
  ustream* stream;
  lstream = vgc_objex_new(heap,lobj_stream,&lobj_stream_type);
  if(!lstream){
    uabort("lstream new error!");
  }

  stream = ustream_new_by_file(USTREAM_OUTPUT,file,URI_REF);
  URI_ERROR;
    uabort(URI_DESC);
  URI_END;

  lstream->stream = stream;
  return lstream;
}
