#include "ulib.h"

ulib udlopen(ustring* name,int mode)
{
#if UOS == WIN
  return LoadLibrary(name->value);
#elif UOS == CYGWIN
  return dlopen(name->value, mode);
#elif UOS == UNX
  return dlopen(name->value,mode);
#endif
}

ulibsym udlsym(ulib handle,ustring* symbol)
{
#if UOS == WIN
  return GetProcAddress(handle, symbol->value);
#elif UOS == CYGWIN
  return dlsym(handle, symbol->value);
#elif UOS == UNX
  return dlsym(handle,symbol->value);
#endif
}

int udlclose(ulib handle)
{
#if UOS == WIN
  return FreeLibrary(handle);
#elif UOS == CYGWIN
  return dlclose(handle);
#elif UOS == UNX
  return dlclose(handle);
#endif
}

const char* udlerror()
{
#if UOS == WIN

#elif UOS == CYGWIN
  return dlerror();
#elif UOS == UNX
  return dlerror();
#endif
}
