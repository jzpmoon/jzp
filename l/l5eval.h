#ifndef _L5EVAL_H_
#define _L5EVAL_H_

#include "vclosure.h"
#include "leval.h"

enum l5kwk{
  #define DF(no,str) no,
  #include "l5kw.h"
  #undef DF
};

UDECLFUN(UFNAME l5startup,
	 UARGS (),
	 URET leval*);

#endif
