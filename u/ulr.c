#include "ulist_tpl.c"
#include "uhstb_tpl.c"
#include "ustack_tpl.c"
#include "ulr.h"

ulist_def_tpl(ulrprodp);
ulist_def_tpl(ulritemp);
uhstb_def_tpl(ulrsetp);
ustack_decl_tpl(ulrsetp);
ustack_def_tpl(ulrsetp);

ulrset* ulrset_new(int inpsym)
{
  ulrset* set;

  set = ualloc(sizeof(ulrset));
  if (set) {
    ugraph_node_init((ugnode*)set,&u_global_allocator);
    set->inpsym = inpsym;
    set->items = ulist_ulritemp_alloc(&u_global_allocator);
  }
  return set;
}

void ulritem_dest(ulritem* item)
{
  ufree(item);
}

void ulrset_dest(ulrset* set)
{
  ugraph_node_clean((ugnode*)set);
  ulist_ulritemp_dest(set->items,ulritem_dest);
  ufree(set);
}

int ulrset_put(ulrset* set,ulritem* item)
{
  return ulist_ulritemp_append(set->items,item);
}

ulritem* ulritem_new(ulrprod* prod,int dot)
{
  ulritem* item;

  item = ualloc(sizeof(ulritem));
  if (item) {
    item->dot = dot;
    item->prod = prod;
  }
  return item;
}

ulrset* ulrgram_start_set_get(ulrgram* gram)
{
  ulrset* itemset;
  ulritem* item;

  if (!gram->accprod) {
    return NULL;
  }
  item = ulritem_new(gram->accprod,0);
  if (!item) {
    return NULL;
  }
  itemset = ulrset_new(ULRINITSYM);
  if (!itemset) {
    return NULL;
  }
  if (ulrset_put(itemset,item)) {
    return NULL;
  }
  return itemset;
}

ulrcoll* ulrcoll_new()
{
  ulrcoll* coll;

  coll = ualloc(sizeof(ulrcoll));
  if (coll) {
    coll->sets = uhstb_ulrsetp_new(-1);
    ugraph_init((ugraph*)coll, &u_global_allocator);
  }
  return coll;
}

int ulritem_comp(ulritem* item1,ulritem* item2)
{
  if (item1->dot > item2->dot) {
    return 1;
  } else if (item1->dot < item2->dot) {
    return -1;
  } else {
    if (item1->prod > item2->prod) {
      return 1;
    } else if (item1->prod > item2->prod) {
      return -1;
    } else {
      return 0;
    }
  }
}

int ulrset_comp(ulrsetp* set1,ulrsetp* set2)
{
  ulist_ulritemp* items1,* items2;
  uset* s1,* s2;
  ucursor c1,c2;
  ulritemp* item1,* item2;
  int retval;

  items1 = (*set1)->items;
  items2 = (*set2)->items;

  if (items1->len > items2->len) {
    return 1;
  } else if (items1->len < items2->len) {
    return -1;
  } else {
    s1 = (uset*)items1;
    s2 = (uset*)items2;
    s1->iterate(&c1);
    s2->iterate(&c2);
    while (1) {
      item1 = s1->next(s1,&c1);
      item2 = s2->next(s2,&c2);
      if (!item1) break;
      retval = ulritem_comp(*item1,*item2);
      if (!retval) return retval;
    }
  }
  return 0;
}

int ulrcoll_put(ulrcoll* coll,ulrset* set)
{
  int retval;
  retval = uhstb_ulrsetp_put(coll->sets,ULRINITSYM,&set,NULL,NULL,
			     ulrset_comp);
  if (!retval) {
    ugraph_node_add((ugraph*)coll,(ugnode*)set);
  }
  return retval;
}

int ulr_closure(ulrgram* gram,ulrset* itemset)
{
  ucursor i,j;
  uset* is,* js;
  ulritemp item,*itemp;
  ulrprodp prod,*prodp;
  ulritem* new_item;

  is = (uset*)itemset->items;
  is->iterate(&i);
  while (1) {
    itemp = is->next(is,&i);
    if (!itemp) break;
    item = *itemp;
    js = (uset*)gram->prods;
    js->iterate(&j);
    while (1) {
      prodp = js->next(js,&j);
      if (!prodp) break;
      prod = *prodp;
      if (item->prod->body[item->dot] == prod->head) {
	new_item = ulritem_new(prod,0);
	if (!new_item) {
	  return -1;
	}
	if (ulrset_put(itemset,new_item)) {
	  return -2;
	}
      }
    }
  }
  return 0;
}

int ulr_goto(ulrgram* gram,
	     ulrcoll* coll,
	     ustack_ulrsetp* stack,
	     ulrset* itemset,
	     int inpsym)
{
  ucursor c;
  uset* s;
  ulritemp item,* itemp;
  ulrset* new_set;
  ulritem* new_item;
  int retval;

  new_set = ulrset_new(inpsym);
  if (!new_set) {
    return -1;
  }
  s = (uset*)itemset->items;
  s->iterate(&c);
  while (1) {
    itemp = s->next(s,&c);
    if (!itemp) break;
    item = *itemp;
    if (item->dot >= item->prod->bdylen) {
      break;
    }
    if (item->prod->body[item->dot + 1] == inpsym) {
      new_item = ulritem_new(item->prod,item->dot + 1);
      if (!new_item) {
	return -1;
      }
      if (ulrset_put(new_set,new_item)) {
	return -1;
      }
    }
  }
  if (ulr_closure(gram,new_set)) {
    return -1;
  }
  retval = ulrcoll_put(coll,new_set);
  if (retval == 1) {
    ulrset_dest(new_set);
  } else if (retval == 0) {
    ustack_ulrsetp_push(stack,new_set);
    ugraph_node_link((ugnode*)itemset,(ugnode*)new_set);
  } else {
    return -1;
  }
  return 0;
}

ulrcoll* ulr0auto(ulrgram* gram)
{
  ulrset* itemset;
  ustack_ulrsetp stack;
  ulrcoll* coll;
  ulritemp item,*itemp;
  uset* s;
  ucursor c;

  ustack_init(ulrsetp,&stack,-1,-1);
  itemset = ulrgram_start_set_get(gram);
  if (!itemset) {
    uabort("grammar start item set get error!");
  }
  if (ulr_closure(gram,itemset)) {
    uabort("item set closure error!");
  }
  coll = ulrcoll_new();
  if (!coll) {
    uabort("item set collection new error!");
  }
  if (ulrcoll_put(coll,itemset)) {
    uabort("item set put to collection error!");
  }
  if (ustack_ulrsetp_push(&stack,itemset)) {
    uabort("push stack error!");
  }
  while (1) {
    if (ustack_ulrsetp_pop(&stack,&itemset)) break;
    s = (uset*)itemset->items;
    s->iterate(&c);
    while (1) {
      itemp = s->next(s,&c);
      if (itemp) break;
      item = *itemp;
      if (item->dot >= item->prod->bdylen) {
	break;
      }
      if (ulr_goto(gram,
		   coll,
		   &stack,
		   itemset,
		   item->prod->body[item->dot + 1])) {
	uabort("item set goto error!");
      }
    }
  }
  return coll;
}
