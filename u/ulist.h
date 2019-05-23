#ifndef _ULIST_H_
#define _ULIST_H_

typedef struct _ulist{
  struct _ulist* prev;
  struct _ulist* next;
  void* value;
} ulist;

void* ulist_append(ulist* list,void* value);

int ulist_length(ulist* list);

#endif
