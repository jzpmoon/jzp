#ifndef _ENV_UNX_H_
#define _ENV_UNX_H_

#include <dlfcn.h>

#define UDIR_SEP '/'

typedef void* ulib;

typedef void* ulibsym;

typedef const char* ulibname;

#define UDLOPEN(ret,name,mode)			\
  ret = dlopen(name,mode)

#define UDLSYM(ret,handle,symbol)		\
  ret = dlsym(handle,symbol)

#define UDLCLOSE(ret,handle)			\
  ret = dlclose(handle)

#define UDLERROR(ret)				\
  ret = dlerror()

#endif
