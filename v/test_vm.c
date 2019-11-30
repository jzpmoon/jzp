#include <stddef.h>

#include "vm.h"
#include "ualloc.h"
#include "uerror.h"

#define INST(code,operand) \
  insts=ulist_append(insts,&((vinst){code,operand}))

int main(){
  
  return 0;
}
