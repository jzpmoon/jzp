#ifndef _L5EVAL_H_
#define _L5EVAL_H_

#include "vclosure.h"
#include "l5macro.h"
#include "leval.h"

typedef struct _l5eval l5eval;

struct _l5eval{
  leval* base_eval;
  vclosure* top_closure;
};

enum l5kwk{
  #define DF(no,str) no,
  #include "l5kw.h"
  #undef DF
};

UDECLFUN(UFNAME l5startup,
         UARGS (char* self_path,
		int vm_gc_asz,
		int vm_gc_ssz,
		int vm_gc_rsz),
         URET l5api l5eval* l5call);

UDECLFUN(UFNAME l5eval_src_load,
         UARGS (l5eval* eval,char* file_path),
         URET l5api int l5call);

UDECLFUN(UFNAME l5eval_lib_load,
         UARGS (l5eval* eval,char* file_path),
         URET l5api int l5call);

UDECLFUN(UFNAME l5eval_conf_load,
         UARGS (l5eval* eval,char* file_path),
         URET l5api int l5call);

#define l5eval_local_name_get(ts,src_name,local_name)		\
  leval_local_name_get(ts,src_name,local_name,'.',".l5");

#endif
