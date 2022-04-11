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

  #define udlinit_register(cstr,dstr)		   \
    BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL,   \
                        _In_ DWORD fdwReason,      \
                        _In_ LPVOID lpvReserved) { \
      dlinit_ft _cstr = cstr;			   \
      dlinit_ft _dstr = dstr;			   \
      if (fdwReason == DLL_PROCESS_ATTACH) {       \
        if (_cstr) _cstr();			   \
      }                                            \
      else					   \
      if (fdwReason == DLL_PROCESS_DETACH) {	   \
	if (_dstr) _dstr();			   \
      }						   \
      return TRUE;                                 \
    }

#elif UOS == CYGWIN

  #include <dlfcn.h>
  #define URTLD_LAZY RTLD_LAZY

  typedef void* ulib;
  typedef void* ulibsym;

  #define udlinit_register(cstr,dstr)		       \
  __attribute__((constructor))			       \
  void _DllMain_cstr(void) {			       \
    dlinit_ft _cstr = cstr;			       \
    if (_cstr) _cstr();				       \
  }						       \
  __attribute__((destructor))			       \
  void _DllMain_dstr(void) {			       \
    dlinit_ft _dstr = dstr;			       \
    if (_dstr) _dstr();				       \
  }


#elif UOS == UNX

  #include <dlfcn.h>
  #define URTLD_LAZY RTLD_LAZY

  typedef void* ulib;
  typedef void* ulibsym;

  #define udlinit_register(cstr,dstr)		       \
  __attribute__((constructor))			       \
  void _DllMain_cstr(void) {			       \
    dlinit_ft _cstr = cstr;			       \
    if (_cstr) _cstr();				       \
  }						       \
  __attribute__((destructor))			       \
  void _DllMain_dstr(void) {			       \
    dlinit_ft _dstr = dstr;			       \
    if (_dstr) _dstr();				       \
  }

#endif

uapi ulib ucall udlopen(ustring* name,int mode);

uapi ulibsym ucall udlsym(ulib handle,ustring* symbol);

uapi int ucall udlclose(ulib handle);

uapi const char* ucall udlerrorstr();

uapi usi32 ucall udlerrorcode();

#endif
