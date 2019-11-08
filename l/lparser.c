#include "uerror.h"
#include "ltoken.h"

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
    }if(c== '\n'){
      ltoken_next_char(ts);
      ltoken_new_line(ts);
      continue;
    }else{
      break;
    }
  }
}

int ltoken_lex_string(ltoken_state* ts,
		      vgc_heap*     heap,
		      vgc_stack*    stack){
  vslot* str;
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
  str = vgc_str_new_by_buff(heap,
			    stack,
			    ts->buff,
			    area_active);
  if(!str){
    uabort("lparse: new vgc_str error!");
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
  str = vstrtb_put_by_buff(ts->buff);
  if(!str){
    uabort("lparse: vstrtb_put error!");
  }
  ts->sym = str;
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
      str = vstrtb_put_by_buff(ts->buff);
      if(!str){
	uabort("lparse: vstrtb_put error!");
      }
      ts->sym = str;
      return ts->token = ltk_identify;
    default:
      ltoken_mark(ts,c);
      ltoken_next_char(ts);
      continue;
    }
  }
}

int ltoken_next(ltoken_state* ts,
		vgc_heap*     heap,
		vgc_stack*    stack){
  int c;
  ltoken_skip_blank(ts);
  c = ltoken_next_char(ts);
  switch(c){
  case '(':
    return ts->token = ltk_left;
  case ')':
    return ts->token = ltk_right;
  case '\'':
    return ts->token = ltk_quote;
  case '"':
    return ltoken_lex_string(ts,heap,stack);
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

vslot* lparser_atom_parse(ltoken_state* ts,
			  vgc_heap*     heap,
			  vgc_stack*    stack){
  int tk = ts->token;
  switch(tk){
  case ltk_string:
    {
      return ts->str;
    }
  case ltk_identify:
    {
      vslot* sym = lsymbol_new(heap,
			       stack,
			       ts->sym);
      if(!sym){
	uabort("lparser_atom_parse error!");
      }
      return sym;
    }
  case ltk_number:
    {
      vslot* num = vslot_num_new(stack,ts->num);
      if(!num){
	uabort("lparser_atom_parse error!");
      }
      return num;
    }
  default:
    ltoken_log(ts);
    uabort("lparser_atom_parse error!");
  }
}

vslot* lparser_exp_parse(ltoken_state* ts,
			 vgc_heap*     heap,
			 vgc_stack*    stack){
  vslot* s_exp = NULL;
  vslot* cons = NULL;
  while(1){
    vslot* car;
    int   tk = ltoken_next(ts,heap,stack);
    switch(tk){
    case ltk_left:
      car = lparser_exp_parse(ts,heap,stack);
      break;
    case ltk_right:
      return s_exp;
    default:
      car = lparser_atom_parse(ts,heap,stack);
    }
    {
      vslot* tmp = lcons_new(heap,stack);
      if(!tmp){
	uabort("lparser_parse error!");
      }
      if(!cons){
	s_exp = tmp;
      }else{
	((lcons*)vslot_ref_get(*cons))
	->cdr = *tmp;
      }
      cons     = tmp;
      ((lcons*)vslot_ref_get(*cons))
      ->car = *car;
    }
  }
}

/*
 * s-exp := ( exp ) | atom
 * exp   := exp exp | s-exp | atom
*/
vslot* lparser_parse(ltoken_state* ts,
		     vgc_heap*     heap,
		     vgc_stack*    stack){
  vslot* s_exp;
  while(1){
    int tk = ltoken_next(ts,heap,stack);
    switch(tk){
    case ltk_left:
      s_exp = lparser_exp_parse(ts,heap,stack);
      return s_exp;
    default:
      s_exp = lparser_atom_parse(ts,heap,stack);
      return s_exp;
    }
  }
}

void ltoken_log(ltoken_state* ts){
  char* sym;
  char* str;
  if(ts->sym){
    sym = ts->sym->value;
  }else{
    sym = "";
  }
  if(ts->str){
    vgc_str* vstr = vslot_ref_get(*ts->str);
    str = vstr->u.c;
  }else{
    str = "";
  }
  ulog ("********token state begin");
  ulog1("token  :%d",ts->token);
  ulog1("symbol :%s",sym);
  ulog1("string :%s",str);
  ulog1("number :%f",ts->num);
  ulog ("********token state   end");
}

void lparser_atom_log(vslot s_exp){
  if(vslot_is_ref(s_exp)){
    vgc_obj* obj = vslot_ref_get(s_exp);
    if(VGCTYPEOF(obj,gc_extend)){
      vgc_objex* objex = (vgc_objex*)obj;
      if(VGCTYPEOFEX(objex,lsymbol_type)){
	lsymbol* sym = (lsymbol*)objex;
	ulog1(" %s",sym->key->value);
      }else{
	ulog(" [objex]");
      }
    }else if(VGCTYPEOF(obj,gc_str)){
      vgc_str* str = (vgc_str*)obj;
      ulog1(" \"%s\"",str->u.c);
    }else{
      ulog(" [obj]");
    }
  }else if(vslot_is_null(s_exp)){
    ulog(" ()");
  }else if(vslot_is_num(s_exp)){
    double num = vslot_num_get(s_exp);
    ulog1(" %f",num);
  }else if(vslot_is_bool(s_exp)){
    int bool = vslot_bool_get(s_exp);
    ulog1(" %d",bool);
  }else{
    ulog("lparser log error!");
  }
}

void lparser_exp_log(vslot s_exp){
  lcons* cons;
  if(vslot_is_ref(s_exp)){
    vgc_obj* obj = vslot_ref_get(s_exp);
    if(VGCTYPEOF(obj,gc_extend)){
      vgc_objex* objex = (vgc_objex*)obj;
      if(VGCTYPEOFEX(objex,lcons_type)){
	cons = (lcons*)objex;
	ulog("(");
	while(1){
	  vslot slot;
	  lparser_exp_log(cons->car);
	  slot = cons->cdr;
	  if(vslot_is_ref(slot)){
	    vgc_obj* obj = vslot_ref_get(slot);
	    if(VGCTYPEOF(obj,gc_extend)){
	      vgc_objex* objex = (vgc_objex*)obj;
	      if(VGCTYPEOFEX(objex,lcons_type)){
		cons = (lcons*)objex;
		continue;
	      }
	    }
	  }else if(vslot_is_null(slot)){
	    ulog(")");
	    break;
	  }
	  lparser_atom_log(slot);
	  break;
	}
	return;
      }
    }
  }
  lparser_atom_log(s_exp);
}
