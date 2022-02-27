#include "ulib.h"

ulib udlopen(ulibname name,int mode)
{
  ulib ret;
  UDLOPEN(ret,name,mode);
  return ret;
}

ulibsym udlsym(ulib handle,char* symbol)
{
  ulibsym ret;
  UDLSYM(ret,handle,symbol);
  return ret;
}

int udlclose(ulib handle)
{
  int ret;
  UDLCLOSE(ret,handle);
  return ret;
}

const char* udlerror()
{
  const char* ret;
  UDLERROR(ret);
  return ret;
}
