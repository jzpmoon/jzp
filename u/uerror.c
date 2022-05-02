#include "umacro.h"
#include <stdarg.h>
#include "uerror.h"

URI_ERR_DEFINE(UERR_SUCC, "U000", "success");
URI_ERR_DEFINE(UERR_ERR, "U001", "error");
URI_ERR_DEFINE(UERR_OFM, "U002", "out of memory");
URI_ERR_DEFINE(UERR_EOF, "U003", "end of file");
URI_ERR_DEFINE(UERR_IOT, "U004", "unknow IO type");
URI_ERR_DEFINE(UERR_DST, "U005", "unknow data source type");

static ulog_conf _log_conf = {NULL,UTRUE,-1};

uapi ulog_infor _uli = {NULL,NULL,1};

uapi int ucall ulog_init(ulog_conf* conf)
{
    int retval = 0;
    FILE* log_fd;

    if (conf)
    {
        if (_uli.log_fd &&
            _uli.log_fd != stdout &&
            fclose(_uli.log_fd) == EOF)
        {
            retval = -1;
        }
        else
        {
            log_fd = fopen(conf->log_fn, "w");
            if (!log_fd)
            {
                retval = -1;
            }
            else
            {
                _uli.log_fd = log_fd;
                _uli.conf = conf;
            }
        }
    }
    else
    {
        _uli.log_fd = stdout;
        _uli.conf = &_log_conf;
    }
    return retval;
}

uapi void ucall uabort(char* msg,...)
{
  FILE* log_fd;
  va_list ap;

  if (!_uli.conf || !_uli.conf->power) {
    va_start(ap, msg);
    vfprintf(stdout, msg, ap);
    va_end(ap);
  } else {
    log_fd = _uli.log_fd;
    if (log_fd) {
      fprintf(log_fd, "%d.<"__DATE__" "__TIME__">", _uli.curr_line);
      va_start(ap, msg);
      vfprintf(log_fd, msg, ap);
      vfprintf(stdout, msg, ap);
      va_end(ap);
      fprintf(log_fd, "\n");
      fflush(log_fd);
    }
  }
  abort();
}

uapi void ucall ulog(char* msg,...)
{
  ulog_conf* conf;
  FILE* log_fd;
  va_list ap;

  conf = _uli.conf;
  if (!conf || !conf->power || !_uli.log_fd) {
    return;
  }
  if (conf->line > 0 &&
      _uli.curr_line % conf->line == 0 &&
      _uli.log_fd != stdout) {
    if (fclose(_uli.log_fd) == EOF) {
      return;
    }
    _uli.log_fd = fopen(conf->log_fn,"w");
  }
  log_fd = _uli.log_fd;
  fprintf(log_fd,"%d.["__DATE__" "__TIME__"]",_uli.curr_line);
  va_start(ap,msg);
  vfprintf(log_fd,msg,ap);
  va_end(ap);
  fprintf(log_fd,"\n");
  fflush(log_fd);
  _uli.curr_line++;
}
