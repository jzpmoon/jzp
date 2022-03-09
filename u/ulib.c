#include "ulib.h"

ulib udlopen(ustring* name,int mode)
{
  return dlopen(name->value,mode);
}

ulibsym udlsym(ulib handle,ustring* symbol)
{
  return dlsym(handle,symbol->value);
}

int udlclose(ulib handle)
{
  return dlclose(handle);
}

const char* udlerror()
{
  return dlerror();
}
