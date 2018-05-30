#ifndef _VOBJ_H_
#define _VOBJ_H_

#include "ustring.h"
#include "udef.h"
#include "vdef.h"

#define vgcArrSize(obj)						\
  sizeof(VgcArray)+(((VgcArray*)obj)->len-1)*sizeof(VgcObj*)
#define vgcNumSize(obj)				\
  sizeof(VgcNum)
#define vgcStringSize(obj)			\
  sizeof(VgcString)

#define gcobjt_defs		      \
  objt_def(gc_arr,vgcArrSize)	      \
  objt_def(gc_num,vgcNumSize)	      \
  objt_def(gc_str,vgcStringSize)

enum gcObj_t{
#define objt_def(t,s) t,
  gcobjt_defs
#undef objt_def
};

#define GCHEADER struct _VgcObj* n;enum gcObj_t t;gcMark_t m;

typedef struct _VgcObj{
  GCHEADER
} VgcObj;

typedef struct _VgcArray{
  GCHEADER
  usize_t len;
  VgcObj* objs[1];
} VgcArray,VgcRootSet;

typedef struct _Value{
  union{
    VInt i;
    VFloat f;
  } u;
} Value;

typedef struct _VgcNum{
 GCHEADER
 Value val;
} VgcNum;

typedef struct _VgcString{
  GCHEADER
  ustring* str;
} VgcString;

#endif
