#ifndef _ULIB_H_
#define _ULIB_H_

#include "umacro.h"
#include "ustring.h"

typedef void (*dlinit_ft)(void);

#if UOS == WIN

  #include <Windows.h>
  #define URTLD_LAZY 0

  typedef HMODULE ulib;
  typedef FARPROC ulibsym;

  #define udlinit_register(callback)               \
    BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL,   \
                        _In_ DWORD fdwReason,      \
                        _In_ LPVOID lpvReserved) { \
      dlinit_ft init = callback;                   \
      if (fdwReason == DLL_PROCESS_ATTACH) {       \
        if (init) init();                          \
      }                                            \
      return TRUE;                                 \
    }

#elif UOS == CYGWIN

  #include <dlfcn.h>
  #define URTLD_LAZY RTLD_LAZY

  typedef void* ulib;
  typedef void* ulibsym;

  #define udlinit_register(callback)                   \
    __attribute__((constructor)) void _DllMain(void) { \
      dlinit_ft init = callback;                       \
      if (init) init();                                \
    }

#elif UOS == UNX

  #include <dlfcn.h>
  #define URTLD_LAZY RTLD_LAZY

  typedef void* ulib;
  typedef void* ulibsym;

  #define udlinit_register(callback)                   \
    __attribute__((constructor)) void _DllMain(void) { \
      dlinit_ft init = callback;                       \
      if (init) init();                                \
    }

#endif

uapi ulib ucall udlopen(ustring* name,int mode);

uapi ulibsym ucall udlsym(ulib handle,ustring* symbol);

uapi int ucall udlclose(ulib handle);

uapi const char* ucall udlerrorstr();

uapi usi32 ucall udlerrorcode();

#endif
