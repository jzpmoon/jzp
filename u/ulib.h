#ifndef _ULIB_H_
#define _ULIB_H_

#include "umacro.h"

ulib udlopen(ulibname name,int mode);

ulibsym udlsym(ulib handle,char* symbol);

int udlclose(ulib handle);

const char* udlerror();

#endif
