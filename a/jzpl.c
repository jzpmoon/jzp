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
  l3eval_load(eval,args[1]);
  return 0;	
UEND

int main (int argc,char** args)
{
  ulog_init("jzpl.log",UTRUE);
  return jzpl(argc,args);
}
