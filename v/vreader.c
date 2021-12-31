#include "vreader.h"

#define VATTR_TABLE_SIZE 17

vreader* vreader_new(ustring_table* symtb,
		     ustring_table* strtb,
		     vattr_init_ft ainit,
		     vast_attr* dattr)
{
  uhstb_vast_attr* attrtb;
  vreader* reader;
  
  attrtb = uhstb_vast_attr_new(VATTR_TABLE_SIZE);
  if (!attrtb) {
    return NULL;
  }

  reader = ualloc(sizeof(vreader));
  if (!reader) {
    uhstb_vast_attr_dest(attrtb,NULL);
    return NULL;
  }

  umem_pool_init(&reader->mp,-1);
  reader->symtb = symtb;
  reader->strtb = strtb;
  reader->attrtb = attrtb;
  reader->ainit = ainit;
  reader->dattr = dattr;

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

vtoken_state* vreader_from(vreader* reader)
{
  uallocator* allocator;
  ustream* stream;
  vtoken_state* ts;

  allocator = &reader->mp.allocator;
  stream = ustream_alloc(allocator,USTREAM_INPUT,USTREAM_FILE);
  if (!stream) {
    return NULL;
  }
  ts = vtoken_state_alloc(allocator,
			  stream,
			  reader->symtb,
			  reader->strtb,
			  reader->attrtb,
			  reader->dattr);
  return ts;
}

void vreader_clean(vreader* reader)
{
  umem_pool_clean(&reader->mp);
}
