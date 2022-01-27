#include "lobj.h"
#include "l3eval.h"
#include "_l3temp.attr"
#include "_l3temp.cfun"

static void l3cfun_init(vcontext* ctx,vmod* mod)
{
  l3cfun_file_concat_init(ctx,mod);
}

static void l3attr_init(vreader* reader)
{
  l3attr_file_concat_init(reader);
}

UDEFUN(UFNAME l3kw_init,
       UARGS (vreader* reader),
       URET static void)
UDECLARE

UBEGIN
  /*keyword init*/
  #define DF(no,str)				       \
  if (vreader_keyword_put(reader,(vast_kw){no,str})) { \
    uabort("keyword put error!");		       \
  }
  #include "l3kw.h"
  #undef DF
UEND

leval* l3startup()
{
  leval* eval;

  eval = lstartup(l3attr_init,l3cfun_init,l3kw_init,
		  vfile2vps,&vast_attr_symcall);
  ulog("l3startup");
  
  return eval;
}
