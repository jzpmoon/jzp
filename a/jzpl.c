#include "uerror.h"
#include "leval.h"

int main (int argc,char** args)
{
  leval* eval;
  
  ulog_init("log.txt");
  if(argc == 1){
    ulog("no input file");
    return 0;
  }
  eval = lstartup();
  leval_load(eval,args[1]);
  return 0;
}
