#ifndef _ULIST_H_
#define _ULIST_H_

#include "ulist_tpl.h"

ulist_decl_tpl(int);
ulist_decl_tpl(long);
ulist_decl_tpl(float);
ulist_decl_tpl(double);
ulist_decl_tpl(uvoidp);

typedef struct _ulist{
  struct _ulist* prev;
  struct _ulist* next;
  void* value;
} ulist;

void* ulist_append(ulist* list,void* value);

int ulist_length(ulist* list);

#endif
