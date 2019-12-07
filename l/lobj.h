#ifndef _LOBJ_H_
#define _LOBJ_H_

#include "ustream.h"
#include "vgc_obj.h"
#include "ltoken.h"

typedef struct _lcons{
  VGCHEADEREX;
  vslot car;
  vslot cdr;
} lobj_cons;

vslot* lobj_cons_new(vgc_heap* heap);

typedef struct _lobj_symbol{
  VGCHEADEREX;
  ustring* key;
} lobj_symbol;

vslot* lobj_symbol_new(vgc_heap* heap,
		       ustring* key);

typedef struct _lobj_stream{
  VGCHEADEREX;
  ustream* stream;
} lobj_stream;

vslot* lobj_stream_new_by_file(vgc_heap* heap,
			       char* file_path);

typedef struct _lobj_parser{
  VGCHEADEREX;
  ltoken_state* ts;
} lobj_parser;

vslot* lobj_parser_new(vgc_heap* heap);

#endif
