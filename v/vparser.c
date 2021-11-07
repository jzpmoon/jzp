#include "uerror.h"
#include "ualloc.h"
#include "uhstb_tpl.c"
#include "vparser.h"

uhstb_def_tpl(vast_attr);

#define VTOKEN_BUFF_SIZE 100

int vast_attr_put_comp(vast_attr* k1,vast_attr* k2){
  return ustring_comp(k1->name,k2->name);
}

int vast_attr_get_comp(vast_attr* k1,vast_attr* k2){
  return ustring_comp(k1->name,k2->name);
}

static ustring* vtoken_state_symbol_finish(vtoken_state* ts){
  ubuffer* buff = ts->buff;
  ustring* str;
  ubuffer_ready_read(buff);
  str = ustring_table_put(ts->symtb,buff->data,buff->limit);
  ubuffer_ready_write(buff);
  return str;
}

static ustring* vtoken_state_string_finish(vtoken_state* ts){
  ubuffer* buff = ts->buff;
  ustring* str;
  ubuffer_ready_read(buff);
  str = ustring_table_add(ts->strtb,buff->data,buff->limit);
  ubuffer_ready_write(buff);
  return str;
}

int vtoken_next_char(vtoken_state* ts){
  URI_DEFINE;

  ts->c = ustream_read_next(ts->stream,URI_REF);
  URI_ERROR;
    URI_CASE(UERR_EOF);
      return ts->c;
    URI_END;
    uabort(URI_DESC);
  URI_END;
  ts->coord.x++;
  return ts->c;
}

int vtoken_look_ahead(vtoken_state* ts){
  URI_DEFINE;

  ts->c = ustream_look_ahead(ts->stream,URI_REF);
  URI_ERROR;
    URI_CASE(UERR_EOF);
      return ts->c;
    URI_END;
    uabort(URI_DESC);
  URI_END;
  return ts->c;
}

#define vtoken_new_line(ts)			\
  ((ts)->coord.x=0,(ts)->coord.y++)

#define vtoken_mark(ts)					\
  if(ubuffer_write_next((ts)->buff,(ts)->c) == -1){	\
    ubuffer_empty((ts)->buff);				\
    return (ts)->token = vtk_bad;			\
  }

#define vtoken_unmark(ts)			\
  ubuffer_ready_write((ts)->buff);

void vtoken_skip_blank(vtoken_state* ts){
  int c;
  while(1){
    c = vtoken_look_ahead(ts);
    if(c == ' ' || c == '\t' || c== '\r'){
      vtoken_next_char(ts);
      continue;
    }else if(c== '\n'){
      vtoken_next_char(ts);
      vtoken_new_line(ts);
      continue;
    }else{
      break;
    }
  }
}

void vtoken_skip_comment(vtoken_state* ts){
  int c;
  vtoken_next_char(ts);
  while (1) {
    c = vtoken_look_ahead(ts);
    if (c == '\n') {
      vtoken_next_char(ts);
      vtoken_new_line(ts);
      break;
    } else if (c == VEOF) {
      break;
    } else {
      vtoken_next_char(ts);
    }
  }
}

void vtoken_skip_block_comment(vtoken_state* ts)
{
  int c;
  vtoken_next_char(ts);
  c = vtoken_look_ahead(ts);
  if (c == '*') {
    vtoken_next_char(ts);
    while (1) {
      c = vtoken_look_ahead(ts);
      if (c == '*') {
	vtoken_next_char(ts);
	c = vtoken_look_ahead(ts);
	if (c == '/') {
	  vtoken_next_char(ts);
	  break;
	} else {
	  continue;
	}
      } else if (c == '\n') {
	vtoken_new_line(ts);
      } else if (c == VEOF) {
	break;
      }
      vtoken_next_char(ts);
    }
  } else {
    uabort("not a block comment!");
  }
}

void vtoken_skip(vtoken_state* ts){
  int c;
  while (1) {
    c = vtoken_look_ahead(ts);
    switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      vtoken_skip_blank(ts);
      break;
    case ';':
      vtoken_skip_comment(ts);
      break;
    case '/':
      vtoken_skip_block_comment(ts);
      break;
    default:
      return;
    }
  }
}

int vtoken_lex_string(vtoken_state* ts){
  ustring* str;
  int      c;
  while(1){
    c = vtoken_next_char(ts);
    if(c == '"'){
      break;
    }
    if(c == VEOF){
      return ts->token = vtk_bad;
    }
    vtoken_mark(ts);
  }
  str = vtoken_state_string_finish(ts);
  if(!str){
    uabort("lparse: vstrtb add error!");
  }
  ts->str = str;
  return ts->token = vtk_string;
}

int vtoken_lex_character(vtoken_state* ts)
{
  int c;
  int v;
  c = vtoken_next_char(ts);
  if (c == '\\') {
    c = vtoken_next_char(ts);
    if (c == '\\') {
      v = '\\';
    } else if (c == '\'') {
      v = '\'';
    } else {
      return ts->token = vtk_bad;
    }
    ts->chara = v;
    return ts->token = vtk_character;
  } else {
    v = c;
    c = vtoken_next_char(ts);
    if (c == '\'') {
      ts->chara = v;
      return ts->token = vtk_character;
    } else {
      return ts->token = vtk_bad;
    }
  }
}

int vtoken_lex_number(vtoken_state* ts){
  ustring* str;
  int      dot = 0;
  int      c;

  vtoken_mark(ts);
  while(1){
    c = vtoken_look_ahead(ts);
    if(c == '.'){
      dot++;
    }
    if(dot > 1){
      return ts->token = vtk_bad;
    }
    if(c < '0' || c > '9'){
      break;
    }
    vtoken_mark(ts);
    vtoken_next_char(ts);
  }
  str = vtoken_state_symbol_finish(ts);
  if(!str){
    uabort("lparse: symtb put error!");
  }
  ts->id = str;
  if (dot == 0) {
    ts->inte = ustring_to_integer(str);
    return ts->token = vtk_integer;
  } else {
    ts->dnum = ustring_to_number(str);
    return ts->token = vtk_number;
  }
}

int vtoken_lex_keyword(vtoken_state* ts)
{
  int retval;

  retval = ustring_charp_comp(ts->id,"nil");
  if (!retval) {
    return ts->token = vtk_nil;
  } else {
    return ts->token = vtk_identify;
  }
}

int vtoken_lex_identify(vtoken_state* ts){
  ustring* str;
  int      c;

  vtoken_mark(ts);
  while(1){
    c = vtoken_look_ahead(ts);
    switch(c){
    case '(':
    case ')':
    case '\'':
    case '"':
    case ' ':
    case '\n':
    case '\t':
    case '\r':
    case VEOF:
      str = vtoken_state_symbol_finish(ts);
      if(!str){
	uabort("lparse: symtb put error!");
      }
      ts->id = str;
      return vtoken_lex_keyword(ts);
    default:
      vtoken_mark(ts);
      vtoken_next_char(ts);
      continue;
    }
  }
}

int vtoken_next(vtoken_state* ts){
  int c;
  
  vtoken_skip(ts);
  c = vtoken_next_char(ts);
  switch(c){
  case '(':
    return ts->token = vtk_left;
  case ')':
    return ts->token = vtk_right;
  case '"':
    return vtoken_lex_string(ts);
  case '\'':
    return vtoken_lex_character(ts);
  case '+':
  case '-':
    c = vtoken_look_ahead(ts);
    if(c < '0' || c > '9'){
      return vtoken_lex_identify(ts);
    }else{
      goto lab_num;
    }
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  lab_num:
    return vtoken_lex_number(ts);
  case VEOF:
    return ts->token = vtk_eof;
  default:
    return vtoken_lex_identify(ts);
  }
}

static int symcall_action(vast_attr_req* req,
			  vast_attr_res* res)
{
  vast_obj* ast_obj;
  vps_cntr* vps;
  vps_cfg* parent;
  vcfg_graph* grp;
  vps_inst* inst;
  vast_obj* obj;
  vast_obj* next;
  vast_symbol* symbol;
  
  ast_obj = req->ast_obj;
  vps = req->vps;
  parent = req->parent;
  if (parent->t != vcfgk_grp) {
    uabort("parent not a graph!");
  }
  grp = (vcfg_graph*)parent;
  obj = vast_car(ast_obj);
  symbol = (vast_symbol*)obj;

  /* push params */
  next = vast_cdr(ast_obj);
  while (next) {
    obj = vast_car(next);
    if (obj->t == vastk_symbol) {
      vast_symbol* sym = (vast_symbol*)obj;
      inst = vps_ipushdt(vps,grp,sym->name);
    }else if(obj->t == vastk_integer){
      vast_integer* inte = (vast_integer*)obj;
      inst = vps_ipushint(vps,grp,inte->name,inte->value);
    }else if(obj->t == vastk_number){
      vast_number* num = (vast_number*)obj;
      inst = vps_ipushnum(vps,grp,num->name,num->value);
    }else if(obj->t == vastk_string){
      vast_string* str = (vast_string*)obj;
      inst = vps_ipushstr(vps,grp,str->value);
    }else{
      uabort("push inst error!");
      inst = NULL;
    }
    vcfg_grp_inst_apd(grp,inst);
    next = vast_cdr(next);
  }
  /* push subr name */
  inst = vps_ipushdt(vps,grp,symbol->name);
  vcfg_grp_inst_apd(grp,inst);
  /* call subr */
  inst = vps_icall(vps);
  vcfg_grp_inst_apd(grp,inst);

  ulog0("symcall");
  VATTR_RETURN_VOID;
}

vast_attr vast_attr_symcall = {NULL,NULL,symcall_action};

vast_obj* vparser_atom_parse(vtoken_state* ts){
  int tk = ts->token;
  switch(tk){
  case vtk_string:
    {
      vast_string* str = vast_string_new(ts,ts->str);
      if(!str){
	uabort("vparser_atom_parse error:new string!");
      }
      return (vast_obj*)str;
    }
  case vtk_character:
    {
      vast_character* chara = vast_character_new(ts,ts->chara);
      if(!chara){
	uabort("vparser_atom_parse error:new vast_character!");
      }
      return (vast_obj*)chara;
    }
  case vtk_identify:
    {
      vast_attr in_attr = {NULL,ts->id,NULL};
      vast_attr* attr;
      vast_symbol* sym;
      uhstb_vast_attr_get(ts->attrtb,
			  ts->id->hash_code,
			  &in_attr,
			  &attr,
			  vast_attr_get_comp);
      if (!attr) {
	attr = &vast_attr_symcall;
      }
      sym = vast_symbol_new(ts,ts->id,attr);
      if(!sym){
	uabort("vparser_atom_parse error:new vast_symbol!");
      }
      return (vast_obj*)sym;
    }
  case vtk_integer:
    {
      vast_integer* integer = vast_integer_new(ts,ts->id,ts->inte);
      if(!integer){
	uabort("vparser_atom_parse error:new vast_integer!");
      }
      return (vast_obj*)integer;
    }
  case vtk_number:
    {
      vast_number* number = vast_number_new(ts,ts->id,ts->dnum);
      if(!number){
	uabort("vparser_atom_parse error:new vast_number!");
      }
      return (vast_obj*)number;
    }
  case vtk_nil:
    {
      vast_nil* nil = vast_nil_new(ts);
      if(!nil){
	uabort("vparser_atom_parse error:new vast_nil!");
      }
      return (vast_obj*)nil;
    }
  default:
    vtoken_log(ts);
    uabort("vparser_atom_parse error!");
  }
  return NULL;
}

vast_obj* vparser_exp_parse(vtoken_state* ts){
  vast_obj* s_exp = NULL;
  vast_cons* cons = NULL;
  while(1){
    vast_obj* car;
    int   tk = vtoken_next(ts);
    switch(tk){
    case vtk_left:
      car = vparser_exp_parse(ts);
      break;
    case vtk_right:
      return s_exp;
    default:
      car = vparser_atom_parse(ts);
    }
    {
      vast_obj* tmp = (vast_obj*)vast_cons_new(ts,NULL,NULL);
      if(!tmp){
	uabort("vparser_parse error!");
      }
      if(!cons){
	s_exp = tmp;
      }else{
	cons->cdr = tmp;
      }
      cons = (vast_cons*)tmp;
      cons->car = car;
    }
  }
}

/*
 * s-exp := ( exp ) | atom
 * exp   := exp exp | s-exp | atom
*/
vast_obj* vparser_parse(vtoken_state* ts){
  vast_obj* s_exp;
  while(1){
    int tk = vtoken_next(ts);
    switch(tk){
    case vtk_left:
      s_exp = vparser_exp_parse(ts);
      return s_exp;
    case vtk_eof:
      return NULL;
    default:
      s_exp = vparser_atom_parse(ts);
      return s_exp;
    }
  }
}

void vtoken_log(vtoken_state* ts){
  char* sym;
  char* str;
  sym = ts->id->value;
  str = ts->str->value;
  ulog0("********token state begin");
  ulog1("token  :%d",ts->token);
  ulog1("symbol :%s",sym);
  ulog1("string :%s",str);
  ulog1("number :%f",ts->dnum);
  ulog0("********token state   end");
}

void vparser_atom_log(vast_obj* s_exp){
  if(s_exp){
    if(s_exp->t == vastk_symbol){
      ulog1("  %s",((vast_symbol*)s_exp)->name->value);
    }else if(s_exp->t == vastk_string){
      ulog1("  \"%s\"",((vast_string*)s_exp)->value->value);
    }else if(s_exp->t == vastk_number){
      ulog1("  %f",((vast_number*)s_exp)->value);
    }else{
      ulog0(" [unkonw]");
    }
  }else{
    ulog0(" ()");
  }
}

void vast_obj_log(vast_obj* ast_obj){
  vast_obj* s_exp = ast_obj;
  vast_cons* cons;
  if(s_exp){
    if(s_exp->t == vastk_cons){
      cons = (vast_cons*)s_exp;
      ulog0("(");
      while(1){
	vast_obj* cdr;
	vast_obj_log(cons->car);
	cdr = cons->cdr;
	if(cdr){
	  if(cdr->t == vastk_cons){
	    cons = (vast_cons*)cdr;
	    continue;
	  }else{
	    vparser_atom_log(cdr);
	    break;
	  }
	}else{
	  ulog0(")");
	  break;
	}
      }
      return;
    }
  }
  vparser_atom_log(s_exp);
}

vast_cons* vast_cons_new(vtoken_state* ts,vast_obj* car,vast_obj* cdr){
  uallocator* allocator;
  vast_cons* cons;

  allocator = ts->allocator;
  cons = allocator->alloc(allocator,sizeof(vast_cons));
  if(cons){
    cons->t = vastk_cons;
    cons->car = car;
    cons->cdr = cdr;
  }
  return cons;
}

vast_symbol* vast_symbol_new(vtoken_state* ts,ustring* name,vast_attr* attr){
  uallocator* allocator;
  vast_symbol* symbol;

  allocator = ts->allocator;
  symbol = allocator->alloc(allocator,sizeof(vast_symbol));
  if(symbol){
    symbol->t = vastk_symbol;
    symbol->name = name;
    symbol->attr = attr;
  }
  return symbol;
}

vast_integer* vast_integer_new(vtoken_state* ts,ustring* name,int inte){
  uallocator* allocator;
  vast_integer* integer;

  allocator = ts->allocator;
  integer = allocator->alloc(allocator,sizeof(vast_integer));
  if(integer){
    integer->t = vastk_integer;
    integer->name = name;
    integer->value = inte;
  }
  return integer;
}

vast_number* vast_number_new(vtoken_state* ts,ustring* name,double dnum){
  uallocator* allocator;
  vast_number* number;

  allocator = ts->allocator;
  number = allocator->alloc(allocator,sizeof(vast_number));
  if(number){
    number->t = vastk_number;
    number->name = name;
    number->value = dnum;
  }
  return number;
}

vast_string* vast_string_new(vtoken_state* ts,ustring* string){
  uallocator* allocator;
  vast_string* str;
  
  allocator = ts->allocator;
  str = allocator->alloc(allocator,sizeof(vast_string));
  if(str){
    str->t = vastk_string;
    str->value = string;
  }
  return str;
}

vast_character* vast_character_new(vtoken_state* ts,int character)
{
  uallocator* allocator;
  vast_character* chara;
  
  allocator = ts->allocator;
  chara = allocator->alloc(allocator,sizeof(vast_character));
  if(chara){
    chara->t = vastk_character;
    chara->value = character;
  }
  return chara;
}

vast_nil* vast_nil_new(vtoken_state* ts)
{
  uallocator* allocator;
  vast_nil* nil;
  
  allocator = ts->allocator;
  nil = allocator->alloc(allocator,sizeof(vast_nil));
  if(nil){
    nil->t = vastk_nil;
  }
  return nil;
}

void vtoken_state_init(vtoken_state* ts)
{
  ubuffer_ready_write(ts->buff);
  ts->token = vtk_bad;
  ts->str = NULL;
  ts->id = NULL;
  ts->dnum = 0;
  ts->bool = 0;
  ts->coord.x = 1;
  ts->coord.y = 1;
}

vtoken_state* vtoken_state_new(ustream* stream,
			       ustring_table* symtb,
			       ustring_table* strtb,
			       uhstb_vast_attr* attrtb,
			       vattr_init_ft ainit)
{
  return vtoken_state_alloc(&u_global_allocator,
			    stream,
			    symtb,
			    strtb,
			    attrtb,
			    ainit);
}

vtoken_state* vtoken_state_alloc(uallocator* allocator,
				 ustream* stream,
				 ustring_table* symtb,
				 ustring_table* strtb,
				 uhstb_vast_attr* attrtb,
				 vattr_init_ft ainit)
{
  vtoken_state* ts;
  ubuffer* buff;
  
  buff = ubuffer_alloc(allocator,VTOKEN_BUFF_SIZE);
  if(!buff){
    return NULL;
  }
  
  ts = allocator->alloc(allocator,sizeof(vtoken_state));
  if (!ts) {
    allocator->free(allocator,buff);
    return NULL;
  }
  ts->allocator = allocator;
  ts->stream = stream;
  ts->buff = buff;
  ts->symtb = symtb;
  ts->strtb = strtb;
  ts->attrtb = attrtb;
  
  vtoken_state_init(ts);
  if (ainit) ainit(ts);
  
  return ts;
}

void vtoken_state_close(vtoken_state* ts)
{
  ustream* stream = ts->stream;
  ustream_close(stream);
  ulog0("stream close");
}

void vtoken_state_reset(vtoken_state* ts,FILE* file){
  ustream* stream = ts->stream;
  ustream_close(stream);
  ulog0("stream close");
  if(ustream_open_by_file(stream,file)){
    uabort("token state file stream open error!");
  }
  ulog0("stream open");
  vtoken_state_init(ts);
}

int vast2vps(vast_attr_req* req,vast_attr_res* res)
{
  vast_obj* ast_obj;
  
  ast_obj = req->ast_obj;  
  switch(ast_obj->t){
  case vastk_cons:{
    vast_obj* obj = vast_car(ast_obj);
    if(obj->t == vastk_symbol){
      vast_symbol* sym = (vast_symbol*)obj;
      if(sym->attr){
	vast_attr* attr;

	attr = sym->attr;
	return (attr->action)(req,res);
      }
    }
  }
    break;
  case vastk_symbol:
    break;
  case vastk_integer:
    break;
  case vastk_number:
    break;
  case vastk_string:
    break;
  default:
    uabort("unknow vast type!");
  }
  VATTR_RETURN_VOID;
}

UDEFUN(UFNAME vfile2vps,
       UARGS (vreader* reader,char* file_path,vps_cntr* vps),
       URET vps_mod*)
UDECLARE
  FILE* file;
  vtoken_state* ts;
  vps_mod* mod;
  vast_obj* ast_obj;
  ustring* mod_name;
  vcfg_graph* grp;
  vps_inst* inst;
  vast_attr_req req;
  vast_attr_res res;
UBEGIN
  ts = vreader_from(reader);
  if (!ts) {
    uabort("reader from error!");
  }
  mod_name = ustring_table_put(ts->symtb,file_path,-1);
  if (!mod_name) {
    uabort("mod name put symtb error!");
  }

  mod = vps_mod_new(vps,mod_name);
  if (!mod) {
    uabort("new mod error!");
  }
  vps_cntr_load(vps,mod);
  
  file = fopen(file_path,"r");
  if(!file){
    uabort("open file error!");
  }
  
  vtoken_state_reset(ts,file);

  grp = vcfg_graph_new(vps,NULL);
  if(!grp){
    uabort("new grp error!");
  }
  grp->parent = (vps_t*)mod;
  grp->scope = VPS_SCOPE_ENTRY;
  mod->entry = grp;
  req.vps = vps;
  req.top = mod;
  req.parent = (vps_cfg*)grp;
  req.reader = reader;
  while(1){
    ast_obj = vparser_parse(ts);
    if (ast_obj == NULL){
      break;
    }
    vast_obj_log(ast_obj);
    req.ast_obj =ast_obj;
    vast2vps(&req,&res);
  }
  vtoken_state_close(ts);

  inst = vps_iretvoid(vps);
  vcfg_grp_inst_apd(grp,inst);

  vcfg_grp_build(vps,grp);
  vcfg_grp_connect(vps,grp);

  vps_mod_loaded(mod);

  return mod;
UEND