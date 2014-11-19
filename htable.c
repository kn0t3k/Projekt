/********* htable.c *********
*							*
* Projekt: IFJ 				*
* Překladač: GCC 4.8.2-7	*
* Datum:   19.11.2014		*
* 							*
****************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"

//-----------------htab_init---------------------------------------------
htab_t* htab_init(unsigned int htab_size)
//funkce alokuje misto pro tabulku, vraci ukazatel na ni
//pri chybe vraci NULL
{
	htab_t *t = malloc(sizeof (struct htab_t) + sizeof ( htab_listitem* [htab_size] )); //alokuje pozadovanou velikost tabulky
	if(t!=NULL) 
	{
		t->htab_size=htab_size; 
		for(int i=0; i<htab_size; i++)
		{
			t->ptr[i]=NULL;			
		}
		return t;
	}
	else
	return NULL;
}


//-----------------htab_free---------------------------------------------
void htab_free(htab_t* t)
//funkce uvolni vsechny polozky v tabulce vcetne tabulky samotne
{
	for(int i=0; i < t->htab_size; i++)
	{
		struct htab_listitem *item = t->ptr[i], *tmp = NULL;
		while(item!=NULL)
		{
			tmp = item;
			item = item->next;
			free(tmp);
		}
		t->ptr[i]=NULL;
	}
	free(t);
	t=NULL;
	return;
}

//-----------------htab_search---------------------------------------------
htab_listitem* htab_search(htab_t *t,const char *key)
//funkce hleda v tabulce pod ukazatelem t prvek s klicem k
//pokud ho najde, vraci ukazatel na nej, pokud ne, vraci NULL
{
	struct htab_listitem *tmp = t->ptr[hash_function(key, SIZE)];
	while(tmp!=NULL)
	{
		if(strcmp(key,tmp->key) == 0) 
		{
			return tmp;
		}		
		tmp = tmp->next;	
	}
	return NULL;	
}



//-----------------htab_add---------------------------------------------
htab_listitem* htab_add(htab_t *t,char *key)
//funkce vztvori a zaradi do tabulky novy prvek s klicem key
//pokud alokace selze, vraci NULL
{
	struct htab_listitem *newitem = (struct htab_listitem*) malloc(sizeof(struct htab_listitem));
	if(newitem==NULL) return NULL;
		
	newitem->key = key;
	//		newitem->type = xxxx;
	//		newitem->value = xxxx;
	
	newitem->next = t->ptr[hash_function(key, SIZE)];
	t->ptr[hash_function(key, SIZE)] = newitem;
	
	return newitem;
}


//-----------------htab_remove---------------------------------------------
void htab_remove(htab_t *t, const char *key)
//funkce odstrani pozadovany prvek z tabulky
{
	int h_num = hash_function(key, SIZE);
	struct htab_listitem *tmp = t->ptr[h_num];
	if(tmp==NULL)
	{
		return; 
	}
	struct htab_listitem *previous = NULL;
	while((tmp!=NULL) && (strcmp(key,tmp->key)!=0) )
	{
		previous = tmp;
		tmp = tmp->next;
	}
	if(tmp==NULL) return; //nebyl nalezen
	if (tmp == t->ptr[h_num]) //je na prvnim miste
	{
		t->ptr[h_num] = tmp->next;
		free(tmp);
	}
	else //je uprostred nebo nakonci
	{
		previous->next = tmp->next;
		free(tmp);		
	}
}
