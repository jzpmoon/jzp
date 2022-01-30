#ifndef _L3EVAL_H_
#define _L3EVAL_H_

#include "leval.h"

enum l3kwk{
  #define DF(no,str) no,
  #include "l3kw.h"
  #undef DF
};

UDECLFUN(UFNAME l3startup,
	 UARGS (),
	 URET leval*);

#endif
