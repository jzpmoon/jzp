#ifndef _LOBJ_H_
#define _LOBJ_H_

#include "vgc_obj.h"

typedef struct _lcons{
  VGCHEADEREX
  vslot car;
  vslot cdr;
} lcons;

extern vgc_objex_t* lcons_type;

vslot* lcons_new(vgc_heap* heap,
		 vgc_stack* stack);

typedef struct _lsymbol{
  VGCHEADEREX
  ustring* key;
  vslot    val;
} lsymbol;

extern vgc_objex_t* lsymbol_type;

vslot* lsymbol_new(vgc_heap*  heap,
		   vgc_stack* stack,
		   ustring*   key);

typedef struct _lstream{
  VGCHEADEREX;
  ustream* stream;
} lstream;

extern vgc_objex_t* lstream_type;

vslot* lstream_new_by_file(vgc_heap*  heap,
			   vgc_stack* stack,
			   char* file_path);

int lstream_dest(lstream* stream);

#endif
