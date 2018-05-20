#ifndef _UASSERT_H_
#define _UASSERT_H_

#include <malloc.h>

#define uassert(test,t,e)			\
  if(test){t}else{e}

#define unew(ptr,size,err)			\
  uassert((ptr=malloc(size))==NULL,err,)

#define ufree(ptr) free(ptr)

#endif
