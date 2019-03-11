#ifndef _VOBJ_H_
#define _VOBJ_H_

#include "udef.h"

enum gcObj_t{gc_obj,gc_arr};

#define ISMARK(O) ((O)->mark.m)
#define GCMARK(O) ((O)->mark.m=1)
#define UNMARK(O) ((O)->mark.m=0)
#define ISADDR(O) ((O)->mark.a)
#define MARKADDR(O) ((O)->mark.a=1)
#define UNADDR(O) ((O)->mark.a=0)
#define GCTYPEOF(O,T) \
  (((O)->mark.t==(T)))
#define INITMARK(O,T) \
  (O)->mark.m=0;      \
  (O)->mark.a=0;      \
  (O)->mark.t=(T)

struct mark_t{
  unsigned char m :1;
  unsigned char a :1;
  unsigned char t :4;
};

#define GCHEADER        \
  struct _VgcObj* addr;	\
  usize_t size;	        \
  struct mark_t mark

typedef struct _VgcObj{
  GCHEADER;
} VgcObj;

typedef struct _VgcArray{
  GCHEADER;
  usize_t len;
  VgcObj* objs[1];
} VgcArray;

#endif
