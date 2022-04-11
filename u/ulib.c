#include "umacro.h"
#include "ulib.h"
#include "uerror.h"

udlinit_register(NULL,NULL)

uapi ulib ucall udlopen(ustring* name,int mode)
{
#if UOS == WIN
  return LoadLibrary(name->value);
#elif UOS == CYGWIN
  return dlopen(name->value, mode);
#elif UOS == UNX
  return dlopen(name->value,mode);
#endif
}

uapi ulibsym ucall udlsym(ulib handle,ustring* symbol)
{
#if UOS == WIN
  return GetProcAddress(handle, symbol->value);
#elif UOS == CYGWIN
  return dlsym(handle, symbol->value);
#elif UOS == UNX
  return dlsym(handle,symbol->value);
#endif
}

uapi int ucall udlclose(ulib handle)
{
#if UOS == WIN
  return FreeLibrary(handle);
#elif UOS == CYGWIN
  return dlclose(handle);
#elif UOS == UNX
  return dlclose(handle);
#endif
}

uapi const char* ucall udlerrorstr()
{
#if UOS == WIN
	return NULL;
#elif UOS == CYGWIN
  return dlerror();
#elif UOS == UNX
  return dlerror();
#endif
}

uapi usi32 ucall udlerrorcode()
{
#if UOS == WIN
	return GetLastError();
#else
	return -1;
#endif
}
