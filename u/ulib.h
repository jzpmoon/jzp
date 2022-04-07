#ifndef _ULIB_H_
#define _ULIB_H_

#include "umacro.h"
#include "ustring.h"

#if UOS == WIN

  #include <Windows.h>
  #define URTLD_LAZY 0

  typedef HMODULE ulib;
  typedef FARPROC ulibsym;

#elif UOS == CYGWIN

  #include <dlfcn.h>
  #define URTLD_LAZY RTLD_LAZY

  typedef void* ulib;
  typedef void* ulibsym;

#elif UOS == UNX

  #include <dlfcn.h>
  #define URTLD_LAZY RTLD_LAZY

  typedef void* ulib;
  typedef void* ulibsym;

#endif

uapi ulib ucall udlopen(ustring* name,int mode);

uapi ulibsym ucall udlsym(ulib handle,ustring* symbol);

uapi int ucall udlclose(ulib handle);

uapi const char* ucall udlerrorstr();

uapi usi32 ucall udlerrorcode();

#endif
