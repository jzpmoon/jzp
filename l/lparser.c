#include "uerror.h"
#include "ualloc.h"
#include "uhstb_tpl.c"
#include "lparser.h"
#include "lobj.h"

uhstb_def_tpl(last_attr);

#define LTOKEN_BUFF_SIZE 100

static ustring* ltoken_state_symbol_finish(ltoken_state* ts){
  ubuffer* buff = ts->buff;
  ustring* str;
  ubuffer_ready_read(buff);
  str = ustring_table_put(ts->symtb,buff->data,buff->limit);
  ubuffer_ready_write(buff);
  return str;
}

static ustring* ltoken_state_string_finish(ltoken_state* ts){
  ubuffer* buff = ts->buff;
  ustring* str;
  ubuffer_ready_read(buff);
  str = ustring_table_add(ts->strtb,buff->data,buff->limit);
  ubuffer_ready_write(buff);
  return str;
}

int ltoken_next_char(ltoken_state* ts){
  URI_DEFINE;
  int c;
  c = ustream_read_next(ts->stream,URI_REF);
  URI_ERROR;
    URI_CASE(UERR_EOF);
      return c;
    URI_END;
    uabort(URI_DESC);
  URI_END;
  ts->coord.x++;
  return c;
}

int ltoken_look_ahead(ltoken_state* ts){
  URI_DEFINE;
  int c;
  c = ustream_look_ahead(ts->stream,URI_REF);
  URI_ERROR;
    URI_CASE(UERR_EOF);
      return c;
    URI_END;
    uabort(URI_DESC);
  URI_END;
  return c;
}

#define ltoken_new_line(ts)			\
  ((ts)->coord.x=0,(ts)->coord.y++)

#define ltoken_mark(ts,c)			\
  if(ubuffer_write_next((ts)->buff,c) == -1){	\
    ubuffer_empty((ts)->buff);			\
    return (ts)->token = ltk_bad;		\
  }

void ltoken_skip_blank(ltoken_state* ts){
  int c;
  while(1){
    c = ltoken_look_ahead(ts);
    if(c == ' ' || c == '\t' || c== '\r'){
      ltoken_next_char(ts);
      continue;
    }else if(c== '\n'){
      ltoken_next_char(ts);
      ltoken_new_line(ts);
      continue;
    }else{
      break;
    }
  }
}

void ltoken_skip_comment(ltoken_state* ts){
  int c;
  while (1) {
    c = ltoken_look_ahead(ts);
    if (c == ';') {
      ltoken_next_char(ts);
      while (1) {
	c = ltoken_look_ahead(ts);
	if (c == '\n') {
	  ltoken_next_char(ts);
	  ltoken_new_line(ts);
	  break;
	} else if (c == LEOF) {
	  break;
	} else {
	  ltoken_next_char(ts);
	}
      }
    } else {
      break;
    }
  }
}

void ltoken_skip(ltoken_state* ts){
  int c;
  while (1) {
    c = ltoken_look_ahead(ts);
    switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      ltoken_skip_blank(ts);
      break;
    case ';':
      ltoken_skip_comment(ts);
      break;
    default:
      return;
    }
  }
}

int ltoken_lex_string(ltoken_state* ts){
  ustring* str;
  int      c;
  while(1){
    c = ltoken_next_char(ts);
    if(c == '"'){
      break;
    }
    if(c == LEOF){
      return ts->token = ltk_bad;
    }
    ltoken_mark(ts,c);
  }
  str = ltoken_state_string_finish(ts);
  if(!str){
    uabort("lparse: vstrtb add error!");
  }
  ts->str = str;
  return ts->token = ltk_string;
}

int ltoken_lex_character(ltoken_state* ts)
{
  int c;
  int v;
  c = ltoken_next_char(ts);
  if (c == '\\') {
    c = ltoken_next_char(ts);
    if (c == '\\') {
      v = '\\';
    } else if (c == '\'') {
      v = '\'';
    } else {
      return ts->token = ltk_bad;
    }
    ts->chara = v;
    return ts->token = ltk_character;
  } else {
    v = c;
    c = ltoken_next_char(ts);
    if (c == '\'') {
      ts->chara = v;
      return ts->token = ltk_character;
    } else {
      return ts->token = ltk_bad;
    }
  }
}

int ltoken_lex_number(ltoken_state* ts){
  ustring* str;
  int      dot = 0;
  int      c;
  while(1){
    c = ltoken_look_ahead(ts);
    if(c == '.'){
      dot++;
    }
    if(dot > 1){
      return ts->token = ltk_bad;
    }
    if(c < '0' || c > '9'){
      break;
    }
    ltoken_mark(ts,c);
    ltoken_next_char(ts);
  }
  str = ltoken_state_symbol_finish(ts);
  if(!str){
    uabort("lparse: symtb put error!");
  }
  ts->id = str;
  if (dot == 0) {
    ts->inte = ustring_to_integer(str);
    return ts->token = ltk_integer;
  } else {
    ts->dnum = ustring_to_number(str);
    return ts->token = ltk_number;
  }
}

int ltoken_lex_identify(ltoken_state* ts){
  ustring* str;
  int      c;
  while(1){
    c = ltoken_look_ahead(ts);
    switch(c){
    case '(':
    case ')':
    case '\'':
    case '"':
    case ' ':
    case '\n':
    case '\t':
    case '\r':
    case LEOF:
      str = ltoken_state_symbol_finish(ts);
      if(!str){
	uabort("lparse: symtb put error!");
      }
      ts->id = str;
      return ts->token = ltk_identify;
    default:
      ltoken_mark(ts,c);
      ltoken_next_char(ts);
      continue;
    }
  }
}

int ltoken_next(ltoken_state* ts){
  int c;
  
  ltoken_skip(ts);
  
  c = ltoken_next_char(ts);
  switch(c){
  case '(':
    return ts->token = ltk_left;
  case ')':
    return ts->token = ltk_right;
  case '"':
    return ltoken_lex_string(ts);
  case '\'':
    return ltoken_lex_character(ts);
  case '+':
  case '-':
    ltoken_mark(ts,c);
    c = ltoken_look_ahead(ts);
    if(c < '0' || c > '9'){
      return ltoken_lex_identify(ts);
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
    ltoken_mark(ts,c);
  lab_num:
    return ltoken_lex_number(ts);
  case LEOF:
    return ts->token = ltk_eof;
  default:
    ltoken_mark(ts,c);
    return ltoken_lex_identify(ts);
  }
}

static int symcall_action(last_attr_req* req,
			  last_attr_res* res)
{
  last_obj* ast_obj;
  vps_cntr* vps;
  vps_cfg* parent;
  vcfg_block* blk;
  vcfg_graph* grp;
  vps_inst* inst;
  last_obj* obj;
  last_obj* next;
  last_symbol* symbol;
  
  ast_obj = req->ast_obj;
  vps = req->vps;
  parent = req->parent;
  if (parent->t != vcfgk_grp) {
    uabort("parent not a graph!");
  }
  grp = (vcfg_graph*)parent;
  obj = last_car(ast_obj);
  symbol = (last_symbol*)obj;
  blk = vcfg_block_new(vps);
  if(!blk){
    uabort("new blk error!");
  }
  blk->parent = (vps_t*)grp;
  next = last_cdr(ast_obj);
  while (next) {
    obj = last_car(next);
    if (obj->t == lastk_symbol) {
      last_symbol* sym = (last_symbol*)obj;
      inst = vps_ipushdt(vps,grp,sym->name);
    }else if(obj->t == lastk_integer){
      last_integer* inte = (last_integer*)obj;
      inst = vps_ipushint(vps,grp,inte->name,inte->value);
    }else if(obj->t == lastk_number){
      last_number* num = (last_number*)obj;
      inst = vps_ipushnum(vps,grp,num->name,num->value);
    }else if(obj->t == lastk_string){
      last_string* str = (last_string*)obj;
      inst = vps_ipushstr(vps,grp,str->value);
    }else{
      uabort("push inst error!");
      inst = NULL;
    }
    vcfg_blk_apd(blk,inst);
    next = last_cdr(next);
  }
  inst = vps_ipushdt(vps,grp,symbol->name);
  vcfg_blk_apd(blk,inst);
  inst = vps_icall(vps);
  vcfg_blk_apd(blk,inst);

  ulog("symcall");
  LATTR_RETURN(lar_vps_apd,blk);
}

last_attr last_attr_symcall = {NULL,NULL,symcall_action};

last_obj* lparser_atom_parse(ltoken_state* ts){
  int tk = ts->token;
  switch(tk){
  case ltk_string:
    {
      last_string* str = last_string_new(ts,ts->str);
      if(!str){
	uabort("lparser_atom_parse error:new string!");
      }
      return (last_obj*)str;
    }
  case ltk_character:
    {
      last_character* chara = last_character_new(ts,ts->chara);
      if(!chara){
	uabort("lparser_atom_parse error:new last_character!");
      }
      return (last_obj*)chara;
    }
  case ltk_identify:
    {
      last_attr in_attr = {NULL,ts->id,NULL};
      last_attr* attr;
      last_symbol* sym;
      uhstb_last_attr_get(ts->attrtb,
			  ts->id->hash_code,
			  &in_attr,
			  &attr,
			  last_attr_get_comp);
      if (!attr) {
	attr = &last_attr_symcall;
      }
      sym = last_symbol_new(ts,ts->id,attr);
      if(!sym){
	uabort("lparser_atom_parse error:new last_symbol!");
      }
      return (last_obj*)sym;
    }
  case ltk_integer:
    {
      last_integer* integer = last_integer_new(ts,ts->id,ts->inte);
      if(!integer){
	uabort("lparser_atom_parse error:new last_integer!");
      }
      return (last_obj*)integer;
    }
  case ltk_number:
    {
      last_number* number = last_number_new(ts,ts->id,ts->dnum);
      if(!number){
	uabort("lparser_atom_parse error:new last_number!");
      }
      return (last_obj*)number;
    }
  default:
    ltoken_log(ts);
    uabort("lparser_atom_parse error!");
  }
  return NULL;
}

last_obj* lparser_exp_parse(ltoken_state* ts){
  last_obj* s_exp = NULL;
  last_cons* cons = NULL;
  while(1){
    last_obj* car;
    int   tk = ltoken_next(ts);
    switch(tk){
    case ltk_left:
      car = lparser_exp_parse(ts);
      break;
    case ltk_right:
      return s_exp;
    default:
      car = lparser_atom_parse(ts);
    }
    {
      last_obj* tmp = (last_obj*)last_cons_new(ts,NULL,NULL);
      if(!tmp){
	uabort("lparser_parse error!");
      }
      if(!cons){
	s_exp = tmp;
      }else{
	cons->cdr = tmp;
      }
      cons = (last_cons*)tmp;
      cons->car = car;
    }
  }
}

/*
 * s-exp := ( exp ) | atom
 * exp   := exp exp | s-exp | atom
*/
last_obj* lparser_parse(ltoken_state* ts){
  last_obj* s_exp;
  while(1){
    int tk = ltoken_next(ts);
    switch(tk){
    case ltk_left:
      s_exp = lparser_exp_parse(ts);
      return s_exp;
    case ltk_eof:
      return NULL;
    default:
      s_exp = lparser_atom_parse(ts);
      return s_exp;
    }
  }
}

void ltoken_log(ltoken_state* ts){
  char* sym;
  char* str;
  sym = ts->id->value;
  str = ts->str->value;
  ulog ("********token state begin");
  ulog1("token  :%d",ts->token);
  ulog1("symbol :%s",sym);
  ulog1("string :%s",str);
  ulog1("number :%f",ts->dnum);
  ulog ("********token state   end");
}

void lparser_atom_log(last_obj* s_exp){
  if(s_exp){
    if(s_exp->t == lastk_symbol){
      ulog1("  %s",((last_symbol*)s_exp)->name->value);
    }else if(s_exp->t == lastk_string){
      ulog1("  \"%s\"",((last_string*)s_exp)->value->value);
    }else if(s_exp->t == lastk_number){
      ulog1("  %f",((last_number*)s_exp)->value);
    }else{
      ulog(" [unkonw]");
    }
  }else{
    ulog(" ()");
  }
}

void last_obj_log(last_obj* ast_obj){
  last_obj* s_exp = ast_obj;
  last_cons* cons;
  if(s_exp){
    if(s_exp->t == lastk_cons){
      cons = (last_cons*)s_exp;
      ulog("(");
      while(1){
	last_obj* cdr;
	last_obj_log(cons->car);
	cdr = cons->cdr;
	if(cdr){
	  if(cdr->t == lastk_cons){
	    cons = (last_cons*)cdr;
	    continue;
	  }else{
	    lparser_atom_log(cdr);
	    break;
	  }
	}else{
	  ulog(")");
	  break;
	}
      }
      return;
    }
  }
  lparser_atom_log(s_exp);
}

last_cons* last_cons_new(ltoken_state* ts,last_obj* car,last_obj* cdr){
  uallocator* allocator;
  last_cons* cons;

  allocator = ts->allocator;
  cons = allocator->alloc(allocator,sizeof(last_cons));
  if(cons){
    cons->t = lastk_cons;
    cons->car = car;
    cons->cdr = cdr;
  }
  return cons;
}

last_symbol* last_symbol_new(ltoken_state* ts,ustring* name,last_attr* attr){
  uallocator* allocator;
  last_symbol* symbol;

  allocator = ts->allocator;
  symbol = allocator->alloc(allocator,sizeof(last_symbol));
  if(symbol){
    symbol->t = lastk_symbol;
    symbol->name = name;
    symbol->attr = attr;
  }
  return symbol;
}

last_integer* last_integer_new(ltoken_state* ts,ustring* name,int inte){
  uallocator* allocator;
  last_integer* integer;

  allocator = ts->allocator;
  integer = allocator->alloc(allocator,sizeof(last_integer));
  if(integer){
    integer->t = lastk_integer;
    integer->name = name;
    integer->value = inte;
  }
  return integer;
}

last_number* last_number_new(ltoken_state* ts,ustring* name,double dnum){
  uallocator* allocator;
  last_number* number;

  allocator = ts->allocator;
  number = allocator->alloc(allocator,sizeof(last_number));
  if(number){
    number->t = lastk_number;
    number->name = name;
    number->value = dnum;
  }
  return number;
}

last_string* last_string_new(ltoken_state* ts,ustring* string){
  uallocator* allocator;
  last_string* str;
  
  allocator = ts->allocator;
  str = allocator->alloc(allocator,sizeof(last_string));
  if(str){
    str->t = lastk_string;
    str->value = string;
  }
  return str;
}

last_character* last_character_new(ltoken_state* ts,int character)
{
  uallocator* allocator;
  last_character* chara;
  
  allocator = ts->allocator;
  chara = allocator->alloc(allocator,sizeof(last_character));
  if(chara){
    chara->t = lastk_character;
    chara->value = character;
  }
  return chara;
}

void ltoken_state_init(ltoken_state* ts)
{
  ubuffer_ready_write(ts->buff);
  ts->token = ltk_bad;
  ts->str = NULL;
  ts->id = NULL;
  ts->dnum = 0;
  ts->bool = 0;
  ts->coord.x = 1;
  ts->coord.y = 1;
}

ltoken_state* ltoken_state_new(ustream* stream,
			       ustring_table* symtb,
			       ustring_table* strtb,
			       uhstb_last_attr* attrtb)
{
  return ltoken_state_alloc(&u_global_allocator,
			    stream,
			    symtb,
			    strtb,
			    attrtb);
}

ltoken_state* ltoken_state_alloc(uallocator* allocator,
				 ustream* stream,
				 ustring_table* symtb,
				 ustring_table* strtb,
				 uhstb_last_attr* attrtb)
{
  ltoken_state* ts;
  ubuffer* buff;
  
  buff = ubuffer_alloc(allocator,LTOKEN_BUFF_SIZE);
  if(!buff){
    return NULL;
  }
  
  ts = allocator->alloc(allocator,sizeof(ltoken_state));
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
  
  ltoken_state_init(ts);
  ltoken_state_attr_init(ts);
  
  return ts;
}

void ltoken_state_close(ltoken_state* ts)
{
  ustream* stream = ts->stream;
  ustream_close(stream);
  ulog("stream close");
}

void ltoken_state_reset(ltoken_state* ts,FILE* file){
  ustream* stream = ts->stream;
  ustream_close(stream);
  ulog("stream close");
  if(ustream_open_by_file(stream,file)){
    uabort("token state file stream open error!");
  }
  ulog("stream open");
  ltoken_state_init(ts);
}

int last2vps(last_attr_req* req,last_attr_res* res)
{
  last_obj* ast_obj;
  
  ast_obj = req->ast_obj;  
  switch(ast_obj->t){
  case lastk_cons:{
    last_obj* obj = last_car(ast_obj);
    if(obj->t == lastk_symbol){
      last_symbol* sym = (last_symbol*)obj;
      if(sym->attr){
	last_attr* attr;

	attr = sym->attr;
	return (attr->action)(req,res);
      }
    }
  }
    break;
  case lastk_symbol:
    break;
  case lastk_integer:
    break;
  case lastk_number:
    break;
  case lastk_string:
    break;
  default:
    uabort("unknow last type!");
  }
  LATTR_RETURN_VOID;
}

vps_mod* lfile2vps(lreader* reader,char* file_path,vps_cntr* vps)
{
  FILE* file;
  ltoken_state* ts;
  vps_mod* mod;
  last_obj* ast_obj;
  ustring* mod_name;
  vcfg_graph* grp;
  vcfg_block* blk;
  last_attr_req req;
  last_attr_res res;
  int retval;

  ts = lreader_from(reader);
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
  
  ltoken_state_reset(ts,file);

  grp = vcfg_graph_new(vps);
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
  blk = NULL;
  while(1){
    ast_obj = lparser_parse(ts);
    if (ast_obj == NULL){
      break;
    }
    req.ast_obj =ast_obj;
    retval = last2vps(&req,&res);
    if (retval != 0) {
      if (res.res_type == lar_vps_apd) {
	vps_t* res_vps = res.res_vps;
	if (!res_vps) {
	  uabort("res_vps can not be null!");
	}
	if(res_vps->t != vcfgk_blk){
	  uabort("not a blk!");
	}
	blk = (vcfg_block*)res_vps;
	vcfg_grp_cdapd(vps,grp,(vps_cfg*)res_vps);
      } else {
	uabort("res_type error!");
      }
    }
  }
  ltoken_state_close(ts);
  if (!blk) {
    blk = vcfg_block_new(vps);
    if(!blk){
      uabort("new blk error!");
    }
    blk->parent = (vps_t*)grp;
    vcfg_grp_cdapd(vps,grp,(vps_cfg*)blk);
  }
  vps_inst* inst = vps_iretvoid(vps);
  vcfg_blk_apd(blk,inst);
  vps_mod_loaded(mod);

  return mod;
}
