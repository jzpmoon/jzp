#include "uerror.h"

URI_ERR_DEFINE(UERR_SUCC, "U000", "success");
URI_ERR_DEFINE(UERR_ERR, "U001", "error");
URI_ERR_DEFINE(UERR_OFM, "U002", "out of memory");
URI_ERR_DEFINE(UERR_EOF, "U003", "end of file");
URI_ERR_DEFINE(UERR_IOT, "U004", "unknow IO type");
URI_ERR_DEFINE(UERR_DST, "U005", "unknow data source type");

typedef struct _ulog_infor{
  FILE* log_fd;
  char* log_fn;
} ulog_infor;

static ulog_infor _uli = {NULL,NULL};

int ulog_init(char* log_fn)
{
  int retval = 0;
  FILE* log_fd = fopen(log_fn,"w");
  if (!log_fd) {
    log_fd = stdout;
    retval = 0;
  }
  _uli.log_fd = log_fd;
  _uli.log_fn = log_fn;
  return retval;
}

void ulog(char* msg)
{
  FILE* log_fd = _uli.log_fd;

  if (!log_fd) {
    log_fd = stdout;
  }
  fprintf(log_fd,"%s\n",msg);
}

void ulog1(char* msg,...)
{
  FILE* log_fd = _uli.log_fd;
  va_list ap;
  int retval;

  if (!log_fd) {
    log_fd = stdout;
  }
  va_start(ap,msg);
  retval = vfprintf(log_fd,msg,ap);
  va_end(ap);
  if (retval < 0) {
    fprintf(log_fd,"ulog error!");
  }
  fprintf(log_fd,"\n");
}

