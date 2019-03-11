#ifndef _UALLOC_H_
#define _UALLOC_H_

#include <malloc.h>
#include <stdlib.h>

#define uif(test,t,e)				\
  if(test){t}else{e}

#define unew(ptr,size,err)			\
  uif(((ptr)=malloc(size))==NULL,err,)

#define ufree(ptr) free(ptr)

#define BOUNDARY 8
#define ALIGN(size,align) ((size+align-1)&(~(align-1)))

#define ALIGN_BOUNDARY(size) ALIGN(size,BOUNDARY)

#define uabort() abort()

#endif
