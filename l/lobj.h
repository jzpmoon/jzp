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

void lobj_stream_new_by_file(vgc_heap* heap,
			     char* file_path);

#endif
