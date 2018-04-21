#ifndef _UASSERT_H_
#define _UASSERT_H_

#define uassert(test,t,e)			\
  if(test){t}else{e}

#define unew(ptr,size,err)			\
  uassert((ptr=malloc(size))==NULL,err,)

#endif
