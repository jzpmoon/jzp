#include "lobj.h"
#include "l5eval.h"
#include "_l5temp.attr"
#include "_l5temp.cfun"

static void l5cfun_init(vcontext* ctx,vmod* mod)
{
  l5cfun_file_concat_init(ctx,mod);
}

static void l5attr_init(vreader* reader)
{
  l5attr_file_concat_init(reader);
}

leval* l5startup()
{
  leval* eval;

  eval = lstartup(l5attr_init,
		  l5cfun_init,
		  vclosure2vps,
		  &vclosure_attr_symcall);
  ulog("l5startup");
  
  return eval;
}
