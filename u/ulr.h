#ifndef _ULR_H_
#define _ULR_H_

#include "ulist_tpl.h"
#include "uhstb_tpl.h"
#include "ugraph.h"

#define ULRINITSYM -1

/*production*/
typedef struct _ulrprod{
  int head;
  /*body length*/
  int bdylen;
  int body[1];
} ulrprod,*ulrprodp;

ulist_decl_tpl(ulrprodp);

/*context-free grammar*/
typedef struct _ulrgram{
  /*nonterminals count*/
  int noncnt;
  /*terminals count*/
  int tercnt;
  /*acceptance production*/
  ulrprod* accprod;
  ulist_ulrprodp* prods;
} ulrgram;

/*item*/
typedef struct _ulritem{
  int dot;
  ulrprod* prod;
} ulritem,*ulritemp;

ulist_decl_tpl(ulritemp);

/*item set*/
typedef struct _ulrset{
  UGNODEHEADER;
  /*input symbol*/
  int inpsym;
  ulist_ulritemp* items;
} ulrset,*ulrsetp;

uhstb_decl_tpl(ulrsetp);

/*item set collection*/
typedef struct _ulrcoll{
  UGRAPHHEADER;
  uhstb_ulrsetp* sets;
} ulrcoll;

ulrcoll* ulr0auto(ulrgram* gram);

#endif
