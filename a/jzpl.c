#include "uerror.h"
#include "leval.h"

int main (int argc,char** args) {
  if(argc == 1){
    ulog("no input file");
    return 0;
  }
  lstartup();
  leval_load(args[1]);
  return 0;
}
