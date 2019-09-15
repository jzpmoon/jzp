#ifndef _UALLOC_H_
#define _UALLOC_H_

#include <malloc.h>
#include <stdlib.h>

#define uif(test,t,e)				\
  if(test){t}else{e}

#define unew(ptr,size,err)			\
  uif(((ptr)=malloc(size))==NULL,err,NULL;)

#define ufree(ptr) free(ptr)

#define BOUNDARY (sizeof(void*))
#define ALIGN(size,align) ((size+align-1)&(~(align-1)))

#define ALIGN_BOUNDARY(size) ALIGN(size,BOUNDARY)

#define TYPE_SIZE_OF(stype,mtype,length)	\
  sizeof(stype)+sizeof(mtype)*((length)-1)

#endif
