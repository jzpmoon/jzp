#include "uerror.h"
#include "leval.h"

UDEFUN(UFNAME jzpl,UARGS (int argc,char** args),URET int,
UDECLARE
  leval* eval;
  ulog_init("jzpl.log",UTRUE);
)
UBEGIN
({
  if(argc == 1){
    ulog("no input file!");
    return 0;
  }
  eval = lstartup();
  leval_load(eval,args[1]);
  return 0;	
})

int main (int argc,char** args)
{
  return jzpl(argc,args);
}
