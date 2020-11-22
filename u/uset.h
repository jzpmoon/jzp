#ifndef _USET_H_
#define _USET_H_

struct _uset;

typedef struct _uiterator{
  char pad[32];  
} uiterator;

typedef void (*uset_iterator_ft)(uiterator* iterator);
typedef void*(*uset_next_ft)(struct _uset* set,uiterator* iterator);

#define USETHEADER				\
  uset_iterator_ft iterate;			\
  uset_next_ft next

typedef struct _uset{
  USETHEADER;
} uset;

#endif
