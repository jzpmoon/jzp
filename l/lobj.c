#include <stddef.h>
#include "lobj.h"

vgc_objex_t lobj_stream_type = {"stream"};

lobj_stream* lobj_istream_new_by_file(vgc_heap* heap,
				      ustring* file_path)
{
  URI_DEFINE;
  lobj_stream* lstream;
  ustream* stream;
  lstream = vgc_objvex_new(heap,lobj_stream,&lobj_stream_type);
  if(!lstream){
    uabort("lstream new error!");
  }

  stream = ustream_new_by_file(USTREAM_INPUT,file_path,URI_REF);
  URI_ERROR;
    uabort(URI_DESC);
  URI_END;

  lstream->stream = stream;
  return lstream;
}

lobj_stream* lobj_ostream_new_by_file(vgc_heap* heap,
				      ustring* file_path)
{
  URI_DEFINE;
  lobj_stream* lstream;
  ustream* stream;
  lstream = vgc_objvex_new(heap,lobj_stream,&lobj_stream_type);
  if(!lstream){
    uabort("lstream new error!");
  }

  stream = ustream_new_by_file(USTREAM_OUTPUT,file_path,URI_REF);
  URI_ERROR;
    uabort(URI_DESC);
  URI_END;

  lstream->stream = stream;
  return lstream;
}

lobj_stream* lobj_istream_new_by_fd(vgc_heap* heap,
				    FILE* fd)
{
  URI_DEFINE;
  lobj_stream* lstream;
  ustream* stream;
  lstream = vgc_objvex_new(heap,lobj_stream,&lobj_stream_type);
  if(!lstream){
    uabort("lstream new error!");
  }

  stream = ustream_new_by_fd(USTREAM_INPUT,fd,URI_REF);
  URI_ERROR;
    uabort(URI_DESC);
  URI_END;

  lstream->stream = stream;
  return lstream;  
}

lobj_stream* lobj_ostream_new_by_fd(vgc_heap* heap,
				    FILE* fd)
{
  URI_DEFINE;
  lobj_stream* lstream;
  ustream* stream;
  lstream = vgc_objvex_new(heap,lobj_stream,&lobj_stream_type);
  if(!lstream){
    uabort("lstream new error!");
  }

  stream = ustream_new_by_fd(USTREAM_OUTPUT,fd,URI_REF);
  URI_ERROR;
    uabort(URI_DESC);
  URI_END;

  lstream->stream = stream;
  return lstream;
}

lobj_stream* lobj_istream_new(vcontext* ctx,
			      vgc_string* fn)
{
  ustring* file_path;

  file_path = ustring_table_put(ctx->symtb,fn->u.c,-1);
  if (!file_path) {
    uabort("file path put symtb error!");
  }
  
  return lobj_istream_new_by_file(ctx->heap,file_path);
}

lobj_stream* lobj_ostream_new(vcontext* ctx,
			      vgc_string* fn)
{
  ustring* file_path;

  file_path = ustring_table_put(ctx->symtb,fn->u.c,-1);
  if (!file_path) {
    uabort("file path put symtb error!");
  }
  
  return lobj_ostream_new_by_file(ctx->heap,file_path);
}
