#ifndef _LOBJ_H_
#define _LOBJ_H_

#include "ustream.h"
#include "vgc_obj.h"
#include "ltoken.h"

typedef struct _lobj_stream{
  VGCHEADEREX;
  ustream* stream;
  vslot_define_begin
  /*
   * void member
   */
  vslot_define_end
} lobj_stream;

extern vgc_objex_t lobj_stream_type;

lobj_stream* lobj_istream_new_by_file(vgc_heap* heap,
				      FILE* file);

lobj_stream* lobj_ostream_new_by_file(vgc_heap* heap,
				      FILE* file);

#endif
