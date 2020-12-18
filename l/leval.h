#ifndef _LEVAL_H_
#define _LEVAL_H_

#include "vm.h"

typedef struct _leval{
  vgc_heap* heap;
  vcontext* ctx;
  ltoken_state* state;
} leval;

void lstartup();

void leval(char* file_path);
