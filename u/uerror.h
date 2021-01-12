#ifndef _UERROR_H_
#define _UERROR_H_

#include <stdio.h>
#include <stdlib.h>

#define uabort(msg)		\
  do{				\
    fprintf(stdout,"%s\n",msg); \
    abort();			\
  }while(0)

#define uabort1(msg,a1)		\
  do{				\
    fprintf(stdout,(msg),(a1));	\
    abort();			\
  }while(0)

#ifndef NDEBUG

#define ulog(msg)				\
  do{						\
    fprintf(stdout,(msg));			\
    fprintf(stdout,"\n");			\
  }while(0)

#define ulog1(msg,a1)				\
  do{						\
    fprintf(stdout,(msg),(a1));			\
    fprintf(stdout,"\n");			\
  }while(0)

#else

  #define ulog(msg) ((void)0)
  #define ulog1(msg,a1) ((void)0)

#endif

typedef struct _ureturn_infor{
  char* code;
  char* desc;
} ureturn_infor;

#define URI_ERR_DEFINE(NAME,CODE,DESC)		\
  const ureturn_infor (*NAME)=			\
    &(ureturn_infor){CODE,DESC}

#define URI_ERR_DECL(NAME)			\
  extern const ureturn_infor (*NAME)
    
#define URI_DEFINE				\
  const ureturn_infor* _ri = UERR_SUCC;		\
  const ureturn_infor** const _rip = &_ri

#define URI_REF _rip

#define URI_DECL const ureturn_infor** const _rip

#define URI_ERROR if(UERR_SUCC != *_rip){

#define URI_CASE(CODE) if((CODE) == *_rip){

#define URI_THEN(CODE) }if((CODE) == *_rip){

#define URI_ELSE }else{

#define URI_END }

#define URI_CODE (*_rip)->code

#define URI_DESC (*_rip)->desc

#define URI_RETVAL(RETERR,RETVAL)		\
  *_rip=(RETERR);return (RETVAL)

URI_ERR_DECL(UERR_SUCC);
URI_ERR_DECL(UERR_ERR);
URI_ERR_DECL(UERR_OFM);
URI_ERR_DECL(UERR_EOF);
URI_ERR_DECL(UERR_IOT);
URI_ERR_DECL(UERR_DST);

#endif
