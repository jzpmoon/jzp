#include "uerror.h"
#include "l3eval.h"

UDEFUN(UFNAME jzpl,UARGS (int argc,char** args),URET int)
UDECLARE
  leval* eval;
UBEGIN
  if (argc == 1) {
    ulog("no input file!");
    return 0;
  }
  eval = l3startup();
  leval_load(eval,args[1]);
  return 0;	
UEND

int main (int argc,char** args)
{
  ulog_conf log_conf;

  log_conf.log_fn = "jzpl.log";
  log_conf.power = UTRUE;
  log_conf.line = 100;
  ulog_init(&log_conf);
  
  return jzpl(argc,args);
}
