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

leval* l3startup()
{
  leval* eval;

  eval = lstartup(l3attr_init,l3cfun_init,vfile2vps,&vast_attr_symcall);
  ulog("l3startup");
  
  return eval;
}
