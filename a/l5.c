#include "uerror.h"
#include "l5eval.h"

UDEFUN(UFNAME jzpl,UARGS (int argc,char** args),URET int)
UDECLARE
  l5eval* eval;
  char* path;
  char* conf;
  char* script;
UBEGIN
  if (argc != 4) {
    uabort("args not enough!");
  }
  script = args[1];
  path = args[2];
  conf = args[3];
  eval = l5startup(path);
  l5eval_conf_load(eval,conf);
  l5eval_src_load(eval,script);
  return 0;	
UEND

int main (int argc,char** args)
{
  ulog_init("jzpl.log",UTRUE);
  return jzpl(argc,args);
}
