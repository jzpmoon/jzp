#ifndef _UERROR_H_
#define _UERROR_H_

#include <stdio.h>
#include <stdlib.h>

#define uabort(msg) \
  do{	            \
    printf(msg);    \
    printf("\n");   \
    abort();	    \
  }while(0)

#ifndef NDEBUG

#define ulog1(msg,a1)				\
  do{						\
    printf((msg),(a1));				\
    printf("\n");				\
  }while(0)

#define ulog(msg)				\
  do{						\
    printf(msg);				\
    printf("\n");				\
  }while(0)

#else

  #define ulog(msg) ((void)0)
  #define ulog1(msg,a1) ((void)0)

#endif

#endif
