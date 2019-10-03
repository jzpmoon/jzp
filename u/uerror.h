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

typedef struct _ureturn_infor{
  int   code;
  char* msg;
} ureturn_infor;

#define URI_DECL ureturn_infor* ri

#define UERR_C_ERR  (-1)

#define UERR_C_OFM  (-2)

#define UERR_M_OFM  "over of memory!"

#define UERR_C_SUCC (0)

#define UERR_M_SUCC "success!"

#define URI_SET(c,m)				\
  ri->code=(c);ri->msg=(m)

#define URI_SETRET(c,m,r)			\
  URI_SET(c,m);return (r)

#define URI_ADDRET_SUCC(a)			\
  URI_SETRET(URI_C_SUCC,URI_M_SUCC,a)

#define URI_VAlRET(c,m)				\
  URI_SET(c,m);return c

#define URI_VALRET_SUCC				\
  URI_VALRET(UERR_C_SUCC,UERR_M_SUCC)

#define URI_NILRET(c,m)				\
  URI_SET(c,m);return NULL

#endif
