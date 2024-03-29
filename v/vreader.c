#include "ulist_tpl.c"
#include "vreader.h"

ulist_def_tpl(vast_kw);

vapi vreader* vcall
vreader_new(ustring_table* symtb,
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

vapi vreader* vcall
vreader_easy_new(vattr_init_ft ainit)
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

vapi int vcall
vreader_keyword_put(vreader* reader,vast_kw keyword)
{
  return ulist_vast_kw_append(reader->kws,keyword);
}

vapi vtoken_state* vcall
vreader_from(vreader* reader)
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

vapi void vcall
vreader_clean(vreader* reader)
{
  uallocator* allocator;
  
  allocator = vreader_alloc_get(reader);
  allocator->clean(allocator);
}

vapi ustring* vcall
vreader_path_get(vreader* reader,ustring* name)
{
  ustring* path;
  
  if (reader->fi.dir_name) {
    path = ustring_concat_by_strtb(reader->symtb,
				   reader->fi.dir_name,name);
    if (!path) {
      uabort("mod name concat error!");
    }
  } else {
    path = name;
  }
  return path;
}

vapi int vcall
vreader_fi_init_01(vreader* reader,ustring* file_full_path)
{
  if (!ufile_init_by_strtb(reader->symtb,&reader->fi,file_full_path)) {
    return -1;
  }
  return 0;
}

vapi int vcall
vreader_fi_init_02(vreader* reader,ustring* file_path,ustring* file_name)
{
  ustring* file_full_path;
  file_full_path = ustring_concat_by_strtb(reader->symtb,
					   file_path,file_name);
  if (!file_full_path) {
    return -1;
  }
  if (!ufile_init_by_strtb(reader->symtb,&reader->fi,file_full_path)) {
    return -1;
  }
  return 0;
}

vapi int vcall
vast_attr_call(vast_attr* attr,vast_attr_req* req,vast_attr_res* res)
{
  if (!attr || !attr->action) {
    return -1;
  }
  req->req_from = attr->action;
  return attr->action(req,res);
}
