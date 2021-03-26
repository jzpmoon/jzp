#ifndef _LEVAL_H_
#define _LEVAL_H_

#include "vm.h"
#include "lparser.h"

typedef struct _leval leval;

typedef struct _leval_loader{
  VMOD_LOADER_HEADER;
  leval* eval;
} leval_loader;

struct _leval{
  vgc_heap* heap;
  vcontext* ctx;
  vmod* mod;
  vps_cntr vps;
  lreader* reader;
  leval_loader loader;
};

leval* lstartup();

int leval_load(leval* eval,char* file_path);

#endif
