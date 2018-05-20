#ifndef _VOBJ_H_
#define _VOBJ_H_

#include "ustring.h"
#include "udef.h"
#include "vdef.h"

enum gcObj_t{
  gc_arr,gc_num,gc_str,
};

typedef char gcMark_t;

#define GCHEADER enum gcObj_t t;VgcObj* n;gcMark_t m;

typedef struct _VgcObj{
  GCHEADER
} VgcObj;

typedef struct _VgcArray{
  GCHEADER
  usize_t len;
  VgcObj* objs[1];
} VgcArray,VgcRootSet;

typedef struct _Value{
  VInt i;
  VFloat f;
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
