#ifndef _UERROR_H_
#define _UERROR_H_

#include <stdio.h>
#include <stdlib.h>
#include "udef.h"

typedef struct _ulog_infor{
  FILE* log_fd;
  char* log_fn;
  int power;
} ulog_infor;

extern ulog_infor _uli;

int ulog_init(char* log_fn,int power);

void ulog_enable(int power);

void uabort(char* msg,...);

void ulog(char* msg,...);

#if NDEBUG

# define ulog0(msg)
# define ulog1(msg,a1)
# define ulog2(msg,a1,a2)
# define ulog3(msg,a1,a2,a3)
# define ulog4(msg,a1,a2,a3,a4)
# define ulog_stack_trace(fname)

#else

# define __ulog(fd,body)			\
  do {						\
    if (!_uli.power) {				\
      break;					\
    }						\
    fprintf(fd,"["__DATE__" "__TIME__"]");	\
    body;					\
    fprintf(fd,"\n");				\
    fflush(fd);					\
  } while(0)

# define ulog0(msg)				\
  do {						\
    if (_uli.log_fd) {				\
      __ulog(_uli.log_fd,			\
	     {fprintf(_uli.log_fd,msg);});	\
    } else {					\
      __ulog(stdout,				\
	     {fprintf(stdout,msg);});		\
    }						\
  } while(0)

# define ulog1(msg,a1)				\
  do {						\
    if (_uli.log_fd) {				\
      __ulog(_uli.log_fd,			\
	     {fprintf(_uli.log_fd,msg,a1);});	\
    } else {					\
      __ulog(stdout,				\
	     {fprintf(stdout,msg,a1);});	\
    }						\
  } while(0)

# define ulog2(msg,a1,a2)			\
  do {						\
    if (_uli.log_fd) {				\
      __ulog(_uli.log_fd,			\
	   {fprintf(_uli.log_fd,msg,a1,a2);});	\
    } else {					\
      __ulog(stdout,				\
	     {fprintf(stdout,msg,a1,a2);});	\
    }						\
  } while(0)

# define ulog3(msg,a1,a2,a3)				\
  do {							\
    if (_uli.log_fd) {					\
      __ulog(_uli.log_fd,				\
	     {fprintf(_uli.log_fd,msg,a1,a2,a3);});	\
    } else {						\
      __ulog(stdout,					\
	     {fprintf(stdout,msg,a1,a2,a3);});		\
    }							\
  } while(0)

# define ulog4(msg,a1,a2,a3,a4)				\
  do {							\
    if (_uli.log_fd) {					\
      __ulog(_uli.log_fd,				\
	     {fprintf(_uli.log_fd,msg,a1,a2,a3,a4);});	\
    } else {						\
      __ulog(stdout,					\
	     {fprintf(stdout,msg,a1,a2,a3,a4);});	\
    }							\
  } while(0)

# define ulog_stack_trace(fname)		\
  ulog0("@"__FILE__"*"#fname"@");

#endif

#define UDEFUN(fname,args,retval)		\
  retval fname args				\
  {						\
  ulog_stack_trace(fname);			\
  {

#define UFNAME

#define UARGS

#define URET

#define UDECLARE

#define UBEGIN

#define UEND }}

#define UDECLFUN(fname,args,retval)		\
  retval fname args;

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
