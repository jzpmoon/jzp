#ifndef _LEVAL_H_
#define _LEVAL_H_

#include "vm.h"
#include "vparser.h"

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
  vreader* reader;
  leval_loader loader;
};

leval* l3startup();

int l3eval_load(leval* eval,char* file_path);

void l3attr_init(vreader* reader);

void l3cfun_init(vcontext* ctx,vmod* mod);

#endif
