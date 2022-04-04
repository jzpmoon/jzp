#include "umacro.h"
#include <stdarg.h>
#include "uerror.h"

URI_ERR_DEFINE(UERR_SUCC, "U000", "success");
URI_ERR_DEFINE(UERR_ERR, "U001", "error");
URI_ERR_DEFINE(UERR_OFM, "U002", "out of memory");
URI_ERR_DEFINE(UERR_EOF, "U003", "end of file");
URI_ERR_DEFINE(UERR_IOT, "U004", "unknow IO type");
URI_ERR_DEFINE(UERR_DST, "U005", "unknow data source type");

uapi ulog_infor _uli = {NULL,NULL,UTRUE};

uapi int ucall ulog_init(char* log_fn,int power)
{
  int retval = 0;
  FILE* log_fd = fopen(log_fn,"w");

  if (!log_fd) {
    log_fd = stdout;
    retval = -1;
  }
  _uli.log_fd = log_fd;
  _uli.log_fn = log_fn;
  _uli.power = power;
  return retval;
}

uapi void ucall ulog_enable(int power)
{
  _uli.power = power;
}

uapi void ucall uabort(char* msg,...)
{
  FILE* log_fd = _uli.log_fd;
  va_list ap;

  if (!_uli.power) {
    return;
  }
  if (!log_fd) {
    log_fd = stdout;
  }
  fprintf(log_fd,"<"__DATE__" "__TIME__">");
  va_start(ap,msg);
  vfprintf(log_fd,msg,ap);
  va_end(ap);
  fprintf(log_fd,"\n");
  fflush(log_fd);
  abort();
}

uapi void ucall ulog(char* msg,...)
{
  FILE* log_fd = _uli.log_fd;
  va_list ap;

  if (!_uli.power) {
    return;
  }
  if (!log_fd) {
    log_fd = stdout;
  }
  fprintf(log_fd,"["__DATE__" "__TIME__"]");
  va_start(ap,msg);
  vfprintf(log_fd,msg,ap);
  va_end(ap);
  fprintf(log_fd,"\n");
  fflush(log_fd);
}
