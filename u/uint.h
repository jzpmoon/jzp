#ifndef _UINT_H_
#define _UINT_H_

typedef unsigned char ibase;

typedef unsigned int itmp;

typedef struct _uint{
  unsigned int sign;
  unsigned int len:
  void* data;
} uint;

#endif
