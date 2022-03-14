#include "uerror.h"
#include "ustring.h"
#include "l5eval.h"

UDEFUN(UFNAME jzpl,UARGS (int argc,char** args),URET int)
UDECLARE
  l5eval* eval;
  char* path;
  char* conf;
  char* script;
  int i;
UBEGIN
  for (i = 0; i < argc;i++) {
    if (!ustrcmp(args[i],"--self-path")) {
      path = args[++i];
    } else if (!ustrcmp(args[i],"--conf")) {
      conf = args[++i];
    } else {
      script = args[++i];
    }
  }
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
