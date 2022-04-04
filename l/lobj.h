#ifndef _LOBJ_H_
#define _LOBJ_H_

#include "ustream.h"
#include "vgc_obj.h"
#include "vparser.h"
#include "vcontext.h"

typedef struct _lobj_stream{
  VGCHEADEREX;
  ustream* stream;
} lobj_stream;

extern vgc_objex_t lobj_stream_type;

lobj_stream* lobj_istream_new_by_file(vgc_heap* heap,
				      ustring* file_path);

lobj_stream* lobj_ostream_new_by_file(vgc_heap* heap,
				      ustring* file_path);

lobj_stream* lobj_istream_new_by_fd(vgc_heap* heap,
				    FILE* fd);

lobj_stream* lobj_ostream_new_by_fd(vgc_heap* heap,
				    FILE* fd);

lobj_stream* lobj_istream_new(vcontext* ctx,
			      vgc_string* fn);

lobj_stream* lobj_ostream_new(vcontext* ctx,
			      vgc_string* fn);

#endif
