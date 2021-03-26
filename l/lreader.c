#include "lreader.h"

#define LATTR_TABLE_SIZE 17

lreader* lreader_new(ustring_table* symtb,
		     ustring_table* strtb)
{
  uhstb_last_attr* attrtb;
  lreader* reader;
  
  attrtb = uhstb_last_attr_new(LATTR_TABLE_SIZE);
  if (!attrtb) {
    return NULL;
  }

  reader = ualloc(sizeof(lreader));
  if (!reader) {
    uhstb_last_attr_dest(attrtb,NULL);
    return NULL;
  }

  umem_pool_init(&reader->mp,-1);
  reader->symtb = symtb;
  reader->strtb = strtb;
  reader->attrtb = attrtb;
  
  return reader;
}

ltoken_state* lreader_from(lreader* reader)
{
  uallocator* allocator;
  ustream* stream;
  ltoken_state* ts;

  allocator = &reader->mp.allocator;
  stream = ustream_alloc(allocator,USTREAM_INPUT,USTREAM_FILE);
  if (!stream) {
    return NULL;
  }
  ts = ltoken_state_alloc(allocator,
			  stream,
			  reader->symtb,
			  reader->strtb,
			  reader->attrtb);
  return ts;
}

void lreader_clean(lreader* reader)
{
  umem_pool_clean(&reader->mp);
}
