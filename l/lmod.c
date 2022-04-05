#include "lmmacro.h"
#include "lobj.h"
#include "_ltemp.cfun"

lmapi void lmcall
lcfun_init(vcontext* ctx,vmod* mod)
{
  lcfun_file_concat_init(ctx,mod);
}
