#ifndef _ULIB_H_
#define _ULIB_H_

#include "umacro.h"
#include "ustring.h"

#if UOS == WIN

  #include <Windows.h>
  #define URTLD_LAZY

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

ulib udlopen(ustring* name,int mode);

ulibsym udlsym(ulib handle,ustring* symbol);

int udlclose(ulib handle);

const char* udlerror();

#endif
