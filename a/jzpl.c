#include "uerror.h"
#include "leval.h"

UDEFUN(UFNAME jzpl,UARGS (int argc,char** args),URET int)
UDECLARE
  leval* eval;
UBEGIN
  if(argc == 1){
    ulog("no input file!");
    return 0;
  }
  eval = lstartup();
  leval_load(eval,args[1]);
  return 0;	
UEND

int main (int argc,char** args)
{
  ulog_init("jzpl.log",UTRUE);
  return jzpl(argc,args);
}
