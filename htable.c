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
#include <stdbool.h>
#include "htable.h"





struct symbol_table* symbol_table_init(int* error){
	
	struct symbol_table* new = (struct symbol_table*) malloc(sizeof(struct symbol_table)); //vytvoreni tabulky symbolu
	if(new == NULL){
		*error = 99;
		return NULL;
	}
	
	new->global = (struct symbol_table_item*) malloc(sizeof(struct symbol_table_item)); //vytvoreni globalni polozky v tabulce
	if(new->global == NULL){
		*error = 99;
		return NULL;
	}
	new->local = new->global; //lokalni neexistuje -> za lokalni se povazuje globalni
	
	
	new->global->next = NULL;
	new->global->table = htab_init(SIZE); //inicializace samotne tabulky
	if(new->global->table == NULL){
		*error = 99;
		return NULL;
	}
	
	*error = 0;
	return new;
}

void add_local_table(struct symbol_table* s_table, int* error){
	if(s_table == NULL || s_table->global==NULL || s_table->local==NULL){ //kontrola argumentu
		*error = 99;
		printf("spatne argumenty v add_local_table()\n");
		return;
	}
		
	struct symbol_table_item* new = (struct symbol_table_item*) malloc(sizeof(struct symbol_table_item)); //alokace nove lokalni polozky ve spojovem seznamu
	if(new == NULL){ //kontrola alokace
		*error = 99;
		return;
	}
		
	new->table = htab_init(SIZE); //nove polozce se vytvori nova hashovaci tabulka
	if(new->table == NULL){
		*error = 99;
		return;
	}
	
	struct symbol_table_item* tmp = s_table->global->next;
	s_table->global->next = new;
	s_table->local = new;
	s_table->local->next = tmp;
	
	*error = 0;
	return;
}	

void remove_local_table(struct symbol_table* s_table, int* error){

	if(s_table == NULL || s_table->global==NULL || s_table->local==NULL){ //kontrola argumentu
		*error = 99;
		printf("spatne argumenty vremove_local_table()\n");
		return;
	}
	
	s_table->local = s_table->global;
	
	*error = 0;
	return; 
}

struct htab_item* add_var(char *name, struct symbol_table* s_table, int* error){
	
	struct htab_item *tmp = s_table->local->table->ptr[hash_function(name, SIZE)];

	if(s_table->local == s_table->global){ 
	//jedna se o globalni promennou == do globalni tabulky
	// nesmi se shodovat s zadnou promennou v globalni tabulce a s zadnou funkci
	
	
	}
	else {
	 //jedna se o lokalni promennou == do lokalni tabulky
	 //nesmi se shodovat s promenou se jmenem ZADNE fce (takova promena tam jiz stejne bude pro return hodnotu)
	 //nesmi se shodovat s zadnou lokalni promennou
	 
	}
	
	
	
	while(tmp!=NULL)
	{
		if(strcmp(name,tmp->name) == 0) 
		{
			*error = SEM_ERROR;  //na stejne urovni se nesmi shodovat jmena promennych
			return NULL;
		}		
		tmp = tmp->next;	
	}	
}


struct htab_item* add_func(char *name, struct symbol_table* s_table, int* error);{
//podivat se do globalni tabulky, pokud uz tam fce je -> error
//jinak vytvorit novou lokalni promennou se stejnym jmenem //nemuze dojit ke kolizi, protoze tabulka bude prazdna (add_local_table se musi volat tesne pred touhle fci)
//
}

//-----------------htab_init---------------------------------------------
htab_t* htab_init(unsigned int htab_size)
//funkce alokuje misto pro tabulku, vraci ukazatel na ni
//pri chybe vraci NULL
{
	htab_t *t = malloc(sizeof (struct htab_t) + sizeof ( htab_item* [htab_size] )); //alokuje pozadovanou velikost tabulky
	if(t!=NULL) 
	{
		t->htab_size = htab_size;
		 
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
		struct htab_item *item = t->ptr[i], *tmp = NULL;
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
htab_item* htab_search(htab_t *t,const char *name)
//funkce hleda v tabulce pod ukazatelem t prvek s klicem k
//pokud ho najde, vraci ukazatel na nej, pokud ne, vraci NULL
{
	struct htab_item *tmp = t->ptr[hash_function(name, SIZE)];
	while(tmp!=NULL)
	{
		if(strcmp(name,tmp->name) == 0) 
		{
			return tmp;
		}		
		tmp = tmp->next;	
	}
	return NULL;	
}



//-----------------htab_add---------------------------------------------
htab_item* htab_add(htab_t *t,char *name)
//funkce vztvori a zaradi do tabulky novy prvek s klicem name
//pokud alokace selze, vraci NULL
{
	struct htab_item *newitem = (struct htab_item*) malloc(sizeof(struct htab_item));
	if(newitem==NULL) return NULL;
		
	newitem->name = name;
	//		newitem->type = xxxx;
	//		newitem->value = xxxx;
	
	newitem->next = t->ptr[hash_function(name, SIZE)];
	t->ptr[hash_function(name, SIZE)] = newitem;
	
	return newitem;
}


//-----------------htab_remove---------------------------------------------
void htab_remove(htab_t *t, const char *name)
//funkce odstrani pozadovany prvek z tabulky
{
	int h_num = hash_function(name, SIZE);
	struct htab_item *tmp = t->ptr[h_num];
	if(tmp==NULL)
	{
		return; 
	}
	struct htab_item *previous = NULL;
	while((tmp!=NULL) && (strcmp(name,tmp->name)!=0) )
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
