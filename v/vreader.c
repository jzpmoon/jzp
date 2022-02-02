#include "ulist_tpl.c"
#include "vreader.h"

ulist_def_tpl(vast_kw);

vreader* vreader_new(ustring_table* symtb,
		     ustring_table* strtb,
		     vattr_init_ft ainit,
		     vast_attr* dattr)
{
  uhstb_vast_attr* attrtb;
  ulist_vast_kw* kws;
  vreader* reader;
  
  attrtb = uhstb_vast_attr_new(-1);
  if (!attrtb) {
    return NULL;
  }
  kws = ulist_vast_kw_new();
  if (!kws) {
    uhstb_vast_attr_dest(attrtb,NULL);
    return NULL;
  }

  reader = ualloc(sizeof(vreader));
  if (!reader) {
    uhstb_vast_attr_dest(attrtb,NULL);
    ulist_vast_kw_dest(kws,NULL);
    return NULL;
  }

  umem_pool_init(&reader->mp,-1);
  reader->symtb = symtb;
  reader->strtb = strtb;
  reader->attrtb = attrtb;
  reader->ainit = ainit;
  reader->dattr = dattr;
  reader->kws = kws;
  reader->fi.file_path = NULL;
  reader->fi.file_name = NULL;
  reader->fi.dir_name = NULL;

  if (ainit) ainit(reader);
  
  return reader;
}

vreader* vreader_easy_new(vattr_init_ft ainit)
{
  ustring_table* symtb;
  ustring_table* strtb;
  
  symtb = ustring_table_new(-1);
  if(!symtb){
    uabort("vcontext_new:symtb new error!");
  }

  strtb = ustring_table_new(-1);
  if(!strtb){
    uabort("vcontext_new:strtb new error!");
  }
  
  return vreader_new(symtb,strtb,ainit,NULL);
}

int vreader_keyword_put(vreader* reader,vast_kw keyword)
{
  return ulist_vast_kw_append(reader->kws,keyword);
}

vtoken_state* vreader_from(vreader* reader)
{
  uallocator* allocator;
  ustream* stream;
  vtoken_state* ts;

  allocator = vreader_alloc_get(reader);
  stream = ustream_alloc(allocator,USTREAM_INPUT,USTREAM_FILE);
  if (!stream) {
    return NULL;
  }
  ts = vtoken_state_alloc(allocator,
			  stream,
			  reader);
  return ts;
}

void vreader_clean(vreader* reader)
{
  uallocator* allocator;
  
  allocator = vreader_alloc_get(reader);
  allocator->clean(allocator);
}
