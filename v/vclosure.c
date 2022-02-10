#include "ulist_tpl.c"
#include "vparser.h"
#include "vclosure.h"

ulist_def_tpl(vclosurep);

UDEFUN(UFNAME vclosure_new,
       UARGS (vps_cntr* vps),
       URET vclosure*)
UDECLARE
  uallocator* allocator;
  vclosure* closure;
  ulist_vps_datap* fields;
  vcfg_graph* init;
  ulist_vclosurep* childs;
UBEGIN
  allocator = vps_cntr_alloc_get(vps);
  closure = allocator->alloc(allocator,sizeof(vclosure));
  if (closure) {
    fields = ulist_vps_datap_alloc(allocator);
    if (!fields) {
      goto err;
    }
    init = vcfg_graph_new(vps,NULL);
    if (!init) {
      goto err;
    }
    childs = ulist_vclosurep_alloc(allocator);
    if (!childs) {
      goto err;
    }
    closure->closure_name = NULL;
    closure->fields = fields;
    closure->init = init;
    closure->parent = NULL;
    closure->childs = childs;
    closure->closure_type = VCLOSURE_TYPE_NONE;
    closure->store_type = VCLOSURE_STORE_IMPL;
  }

  return closure;
 err:
  allocator->clean(allocator);
  return NULL;
UEND

UDEFUN(UFNAME vclosure2mod,
       UARGS (vclosure* closure,vps_cntr* vps,vps_mod* mod),
       URET static void)
UDECLARE
  uset* set;
  ucursor c;
  vps_mod* new_mod;
UBEGIN
  if (vclosure_isdecl(closure)) {
    return;
  }
  switch (closure->closure_type) {
  case VCLOSURE_TYPE_NONE:
    new_mod = NULL;
    break;
  case VCLOSURE_TYPE_FILE:
    new_mod = vps_mod_new(vps,closure->closure_name);
    if (!new_mod) {
      uabort("mod new error!");
    }
    goto label;
  case VCLOSURE_TYPE_MAIN:
    new_mod = vps_mod_new(vps,closure->closure_name);
    if (!new_mod) {
      uabort("mod new error!");
    }
    vps_mod_entry_set(new_mod);
  label:
    vps_cntr_load(vps,new_mod);
    vps_mod_loaded(new_mod);
    /*fields*/
    set = (uset*)closure->fields;
    set->iterate(&c);
    while (1) {
      vps_datap* fieldp = set->next(set,&c);
      vps_data* field;
      if (!fieldp) {
	break;
      }
      field = *fieldp;
      if (field->scope != VPS_SCOPE_DECL) {
	vps_mod_data_put(new_mod,field);
      }
    }
    /*entry*/
    new_mod->entry = closure->init;
    break;
  case VCLOSURE_TYPE_FUNC:
    new_mod = mod;
    vps_mod_code_put(new_mod,closure->init);
    break;
  default:
    uabort("unknow closure type!");
  }
  set = (uset*)closure->childs;
  set->iterate(&c);
  while (1) {
    vclosurep* childp = set->next(set,&c);
    vclosure* child;
    if (!childp) {
      break;
    }
    child = *childp;
    /*childs*/
    if (child) {
      vclosure2mod(child,vps,new_mod);
    }
  }
UEND

UDEFUN(UFNAME vfile2closure,
       UARGS (vclosure* parent,vreader* reader,ustring* file_path,
	      vps_cntr* vps,int closure_type),
       URET vclosure*)
UDECLARE
  vclosure* closure;
  vps_closure_req req;
  vast_attr_res res;
  vcfg_graph* init;
  vps_inst* inst;
UBEGIN
  closure = vclosure_new(vps);
  if (!closure) {
    uabort("closure new error!");
  }
  closure->closure_type = closure_type;
  closure->closure_name = file_path;
  if (parent) {
    if (vclosure_child_add(parent,closure)) {
      uabort("parent closure add child error!");
    }
  }
  init = closure->init;
  init->scope = VPS_SCOPE_ENTRY;
  req.vps = vps;
  req.closure = closure;
  req.reader = reader;
  if (vfile2obj(reader,file_path,(vast_attr_req*)&req,&res)) {
    uabort("file2obj error!");
  }

  inst = vps_iretvoid(vps);
  vcfg_grp_inst_apd(init,inst);
  vcfg_grp_build(vps,init);
  vcfg_grp_connect(vps,init);

  return closure;
UEND

UDEFUN(UFNAME vclosure2vps,
       UARGS (vreader* reader,char* file_path,vps_cntr* vps),
       URET vps_mod*)
UDECLARE
  ustring* mod_name;
  vclosure* top_closure;
  vclosure* closure;
UBEGIN
  top_closure = vclosure_new(vps);
  if (!top_closure) {
    uabort("top closure new error!");
  }
  mod_name = ustring_table_put(reader->symtb,file_path,-1);
  if (!mod_name) {
    uabort("mod name put symtb error!");
  }

  closure = vfile2closure(top_closure,reader,mod_name,vps,
			  VCLOSURE_TYPE_MAIN);
  vclosure2mod(top_closure,vps,NULL);

  return vps->entry;
UEND

UDEFUN(UFNAME vclosure_field_get,
       UARGS (vclosure* closure,ustring* name),
       URET vps_data*)
UDECLARE
  uset* fields;
  uset* childs;
  ucursor c;
  ucursor i;
  vps_data* data;
UBEGIN
  /*find local fields*/
  data = vcfg_grp_dtget(closure->init,name);
  if (data) {
    return data;
  }

  /*find closure fields*/
  fields = (uset*)closure->fields;
  fields->iterate(&c);
  while (1) {
    vps_datap* dp = fields->next(fields,&c);
    if (!dp) {
      break;
    }
    data = *dp;
    if (!ustring_comp(name,data->name)) {
      return data;
    }
  }

  /*find closure childs*/
  childs = (uset*)closure->childs;
  childs->iterate(&c);
  while (1) {
    vclosurep* childp = childs->next(childs,&c);
    vclosure* child;
    if (!childp) {
      break;
    }
    child = *childp;
    if (vclosure_ismain(child) || vclosure_isfile(child)) {
      /*find child fields*/
      fields = (uset*)child->fields;
      fields->iterate(&i);
      while (1) {
	vps_datap* dp = fields->next(fields,&i);
	if (!dp) {
	  break;
	}
	data = *dp;
	if (!ustring_comp(name,data->name)) {
	  return data;
	}
      }
    }
  }

  /*find parent fields*/
  if (closure->parent) {
    return vclosure_field_get(closure->parent,name);
  } else {
    return NULL;
  }
UEND

UDEFUN(UFNAME vclosure_curr_field_get,
	 UARGS (vclosure* closure,ustring* name),
	 URET vps_data*)
UDECLARE
  uset* fields;
  ucursor c;
  vps_data* data;
UBEGIN
  /*find local fields*/
  data = vcfg_grp_dtget(closure->init,name);
  if (data) {
    return data;
  }

  /*find closure fields*/
  fields = (uset*)closure->fields;
  fields->iterate(&c);
  while (1) {
    vps_datap* dp = fields->next(fields,&c);
    if (!dp) {
      break;
    }
    data = *dp;
    if (!ustring_comp(name,data->name)) {
      return data;
    }
  }
  return NULL;
UEND

UDEFUN(UFNAME vclosure_field_add,
       UARGS (vclosure* closure,vps_data* field),
       URET int)
UDECLARE

UBEGIN
  return ulist_vps_datap_append(closure->fields,field);
UEND

UDEFUN(UFNAME vclosure_child_add,
	 UARGS (vclosure* closure,vclosure* child),
	 URET int)
UDECLARE

UBEGIN
  child->parent = closure;
  return ulist_vclosurep_append(closure->childs,child);
UEND
