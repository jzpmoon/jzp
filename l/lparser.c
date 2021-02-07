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
  ts->num = ustring_to_number(str);
  return ts->token = ltk_number;
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
  case '\'':
    return ts->token = ltk_quote;
  case '"':
    return ltoken_lex_string(ts);
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

last_obj* lparser_atom_parse(ltoken_state* ts){
  int tk = ts->token;
  switch(tk){
  case ltk_string:
    {
      last_string* str = last_string_new(ts,ts->str);
      if(!str){
	uabort("lparser_atom_parse error!");
      }
      return (last_obj*)str;
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
      sym = last_symbol_new(ts,ts->id,attr);
      if(!sym){
	uabort("lparser_atom_parse error!");
      }
      return (last_obj*)sym;
    }
  case ltk_number:
    {
      last_number* num = last_number_new(ts,ts->id,ts->num);
      if(!num){
	uabort("lparser_atom_parse error!");
      }
      return  (last_obj*)num;
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
  ulog1("number :%f",ts->num);
  ulog ("********token state   end");
}

void lparser_atom_log(last_obj* s_exp){
  if(s_exp){
    if(s_exp->t == lastk_symbol){
      ulog1("  %s",((last_symbol*)s_exp)->name->value);
    }else if(s_exp->t == lastk_string){
      ulog1("  \"%s\"",((last_string*)s_exp)->string->value);
    }else if(s_exp->t == lastk_number){
      ulog1("  %f",((last_number*)s_exp)->dnum);
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
  umem_pool* pool;
  last_cons* cons;

  pool = &ts->pool;
  cons = umem_pool_alloc(pool,sizeof(last_cons));
  if(cons){
    cons->t = lastk_cons;
    cons->car = car;
    cons->cdr = cdr;
  }
  return cons;
}

last_symbol* last_symbol_new(ltoken_state* ts,ustring* name,last_attr* attr){
  umem_pool* pool;
  last_symbol* symbol;

  pool = &ts->pool;
  symbol = umem_pool_alloc(pool,sizeof(last_symbol));
  if(symbol){
    symbol->t = lastk_symbol;
    symbol->name = name;
    symbol->attr = attr;
  }
  return symbol;
}

last_number* last_number_new(ltoken_state* ts,ustring* name,double dnum){
  umem_pool* pool;
  last_number* number;
  
  pool = &ts->pool;
  number = umem_pool_alloc(pool,sizeof(last_number));
  if(number){
    number->t = lastk_number;
    number->name = name;
    number->dnum = dnum;
  }
  return number;
}

last_string* last_string_new(ltoken_state* ts,ustring* string){
  umem_pool* pool;
  last_string* str;
  
  pool = &ts->pool;
  str = umem_pool_alloc(pool,sizeof(last_string));
  if(str){
    str->t = lastk_string;
    str->string = string;
  }
  return str;
}

void ltoken_state_init(ltoken_state* ts,
		       ustream* stream){
  ts->stream = stream;
  ubuffer_ready_write(ts->buff);
  ts->token = ltk_bad;
  ts->str = NULL;
  ts->id = NULL;
  ts->num = 0;
  ts->bool = 0;
  ts->coord.x = 1;
  ts->coord.y = 1;
}

#define LATTR_TABLE_SIZE 17

ltoken_state* ltoken_state_new(ustream* stream,
			       ustring_table* symtb,
			       ustring_table* strtb){
  ltoken_state* ts;
  ubuffer* buff;
  uhstb_last_attr* attrtb;
  
  buff = ubuffer_new(LTOKEN_BUFF_SIZE);
  if(!buff){
    goto err;
  }
  
  attrtb = uhstb_last_attr_new(LATTR_TABLE_SIZE);
  if(!attrtb){
    goto err;
  }

  unew(ts,sizeof(ltoken_state),goto err;);
  
  ts->buff = buff;
  ts->symtb = symtb;
  ts->strtb = strtb;
  ts->attrtb = attrtb;

  umem_pool_init(&ts->pool,-1);
  
  ltoken_state_init(ts,stream);
  ltoken_state_attr_init(ts);
  
  return ts;
 err:
  ubuffer_dest(buff);
  return NULL;
}

void ltoken_state_reset(ltoken_state* ts,FILE* file){
  ustream* stream = ts->stream;
  ustream_close(stream);
  ulog("stream close");
  if(ustream_open_by_file(stream,file)){
    uabort("token state file stream open error!");
  }
  ulog("stream open");
  ltoken_state_init(ts,stream);
}

int last2vps(ltoken_state* ts,last_obj* ast_obj,vps_mod* mod){
  switch(ast_obj->t){
  case lastk_cons:{
    last_obj* obj = last_car(ast_obj);
    if(obj->t == lastk_symbol){
      last_symbol* sym = (last_symbol*)obj;
      if(sym->attr){
	last_attr* attr = sym->attr;
	last_attr_res res;
	(attr->action)(mod,NULL,ast_obj,&res);
      }
    }
  }
    break;
  case lastk_symbol:
    break;
  case lastk_number:
    break;
  case lastk_string:
    break;
  default:
    return -1;
  }
  return 0;
}
