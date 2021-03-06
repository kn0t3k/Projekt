/*--------------------------------------
| Projekt: IFJ14						
| Soubor: ial.c						
| Autori: Denk Filip (xdenkf00)		
|	  Jurica Tomas (xjuric22)		
|	  Knotek Martin (xknote11)	
|	  Kohut Jan (xkohut08)		
|	  Piskula David (xpisku02)	
| Prekladac: GCC 4.8.2-7				
| Datum:   5.12.2014					
|--------------------------------------*/

#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdbool.h>
#include "err.h"
#include "ial.h"



//-----------------hash_function---------------------------------------------
unsigned int hash_function(const char *str, unsigned htab_size)
//hashovaci funkce, ktera pro zadany retezec vraci hodnotu z pozadovaneho intervalu
//hashovaci funkce byla prevzata z projektu predmetu IJC
{
 unsigned int h=0;
 const unsigned char *p;
 for(p=(const unsigned char*)str; *p!='\0'; p++)
 		h = 65599*h + *p;
 return h % htab_size;
}

//-----------------symbol_table_init()---------------------------------------------
struct symbol_table* symbol_table_init(){	
	struct symbol_table* new = (struct symbol_table*) malloc(sizeof(struct symbol_table)); //vytvoreni tabulky symbolu
	if(new == NULL){
		return NULL;
	}
	
	new->global = (struct symbol_table_item*) malloc(sizeof(struct symbol_table_item)); //vytvoreni globalni polozky v tabulce
	if(new->global == NULL){
		return NULL;
	}
	new->local = new->global; //lokalni neexistuje -> za lokalni se povazuje globalni
	new->global->item_count = 0;
	
	new->global->next = NULL;
	new->global->table = htab_init(SIZE); //inicializace samotne hashovaci tabulky
	if(new->global->table == NULL){
		return NULL;
	}
	
	
	int error;
	struct htab_item* builtin_func;
	struct htab_item* builtin_var;
	
	
	//--------------------------pridani vestavene fce length(s:string): integer;
	add_local_table(new, &error);
	if(error) return NULL;
	
	builtin_func = add_func("length", new, &error);
	if(builtin_func == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou fci, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_func->type = s_integer;
	builtin_func->initialized = true;
	builtin_func->func_data = (char*)malloc(sizeof(char)*2); //2= 1char(1 parametr) + 1 ukoncovaci '\0'
	if(builtin_func->func_data == NULL){ //kontrola alokace pro retezec
		return NULL;
	}
	strcpy(builtin_func->func_data, "s"); //nakopirovani dat
	
	//--------------------------nastaveni typu promenne pro return hodnotu
	builtin_var = search_var("length", new, &error);
	builtin_var->type = s_integer;
	
	//--------------------------pridani vestavene s:string;
	builtin_var = add_var("s", new, &error);
	if(builtin_var == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou promennou, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_var->type = s_string;
	builtin_var->initialized = true;
	
	remove_local_table(new, &error);
	
	
	//--------------------------pridani vestavene fce copy(s:string; i:integer; n:integer): string;
	add_local_table(new, &error);
	if(error) return NULL;
	
	builtin_func = add_func("copy", new, &error);
	if(builtin_func == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou fci, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_func->type=s_string;
	builtin_func->initialized = true;
	builtin_func->func_data = (char*)malloc(sizeof(char)*4); //4= 3char(3 parametr) + 1 ukoncovaci '\0'
	if(builtin_func->func_data == NULL){ //kontrola alokace pro retezec
		return NULL;
	}
	strcpy(builtin_func->func_data, "sii"); //nakopirovani dat
	
	//--------------------------nastaveni typu promenne pro return hodnotu
	builtin_var = search_var("copy", new, &error);
	builtin_var->type = s_string;
	
	//--------------------------pridani vestavene s:string;
	builtin_var = add_var("s", new, &error);
	if(builtin_var == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou promennou, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_var->type = s_string;
	builtin_var->initialized = true;
	
	//--------------------------pridani vestavene i:integer;
	builtin_var = add_var("i", new, &error);
	if(builtin_var == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou promennou, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_var->type = s_integer;
	builtin_var->initialized = true;
	
	//--------------------------pridani vestavene n:integer;	
	builtin_var = add_var("n", new, &error);
	if(builtin_var == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou promennou, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_var->type = s_integer;
	builtin_var->initialized = true;
	
	remove_local_table(new, &error);
	
	
	//--------------------------pridani vestavene fce find(s:string; search:string;): integer;
	add_local_table(new, &error);
	if(error) return NULL;
	
	builtin_func = add_func("find", new, &error);
	if(builtin_func == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou fci, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_func->type=s_integer;
	builtin_func->initialized = true;
	builtin_func->func_data = (char*)malloc(sizeof(char)*3); //3= 2char(2 parametr) + 1 ukoncovaci '\0'
	if(builtin_func->func_data == NULL){ //kontrola alokace pro retezec
		return NULL;
	}
	strcpy(builtin_func->func_data, "ss"); //nakopirovani dat
	
	//--------------------------nastaveni typu promenne pro return hodnotu
	builtin_var = search_var("find", new, &error);
	builtin_var->type = s_integer;
	
	//--------------------------pridani vestavene s:string;
	builtin_var = add_var("s", new, &error);
	if(builtin_var == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou promennou, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_var->type = s_string;
	builtin_var->initialized = true;
	
	//--------------------------pridani vestavene search:string;
	builtin_var = add_var("search", new, &error);
	if(builtin_var == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou promennou, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_var->type = s_string;
	builtin_var->initialized = true;
	
	remove_local_table(new, &error);
		
		
	//--------------------------pridani vestavene fce sort(s:string): string;
	add_local_table(new, &error);
	if(error) return NULL;
	
	builtin_func = add_func("sort", new, &error);
	if(builtin_func == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou fci, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_func->type=s_string;
	builtin_func->initialized = true;
	builtin_func->func_data = (char*)malloc(sizeof(char)*2); //2= 1char(1 parametr) + 1 ukoncovaci '\0'
	if(builtin_func->func_data == NULL){ //kontrola alokace pro retezec
		return NULL;
	}
	strcpy(builtin_func->func_data, "s"); //nakopirovani dat
	
	//--------------------------nastaveni typu promenne pro return hodnotu
	builtin_var = search_var("sort", new, &error);
	builtin_var->type = s_string;
	
	//--------------------------pridani vestavene s:string;
	builtin_var = add_var("s", new, &error);
	if(builtin_var == NULL){
		//nemuze dojit k zadnemu konfliktu s jinou promennou, neni potreba kontrolovat error, muze nastat pouze typ 99
		return NULL;
	}
	builtin_var->type = s_string;
	builtin_var->initialized = true;
	
	remove_local_table(new, &error);
	
	return new;
}

//-----------------add_local_table---------------------------------------------
void add_local_table(struct symbol_table* s_table, int* error){
	if(s_table == NULL || s_table->global==NULL || s_table->local==NULL){ //kontrola argumentu
		*error = SEM_ERROR;
		printf("spatne argumenty v add_local_table()\n");
		return;
	}
		
	struct symbol_table_item* new = (struct symbol_table_item*) malloc(sizeof(struct symbol_table_item)); //alokace nove lokalni polozky ve spojovem seznamu
	if(new == NULL){ //kontrola alokace
		*error = INTERNAL_ERR;
		return;
	}
		
	new->table = htab_init(SIZE); //nove polozce se vytvori nova hashovaci tabulka
	if(new->table == NULL){
		*error = SEM_ERROR;
		return;
	}
	
	new->item_count = 0;
	
	struct symbol_table_item* tmp = s_table->global->next;
	s_table->global->next = new;
	s_table->local = new;
	s_table->local->next = tmp;
	
	*error = 0;
	return;
}	


//-----------------remove_local_table---------------------------------------------
void remove_local_table(struct symbol_table* s_table, int* error){
	if(s_table == NULL || s_table->global==NULL || s_table->local==NULL){ //kontrola argumentu
		*error = SEM_ERROR;
		printf("spatne argumenty vremove_local_table()\n");
		return;
	}
	
	s_table->local = s_table->global; //loklni ukazetel je shodny s globalnim == jsme na globalni urovni
	
	*error = 0;
	return; 
}


//-----------------add_var---------------------------------------------
struct htab_item* add_var(char *name_notupper, struct symbol_table* s_table, int* error){	
	struct htab_item *tmp = NULL;
	
	int i = 0;	
	//do tabulky se vsechny promenne ukladaji s nazvem z velkych pismen, jazyk je case insensitive a case insensitive hashovaci funkce by ztracela na rychlosti
	char* name = malloc(sizeof(char)*(strlen(name_notupper)+1));
	if(name == NULL){
		*error = INTERNAL_ERR;
		return NULL;
	}
	while(name_notupper[i]){ //prekopirovani retezce a prevod na upper case
		name[i]=toupper(name_notupper[i]);
		i++;
	}
	name[i]= '\0';
	
	
	if(s_table->local == s_table->global){ 
	//jedna se o globalni promennou == do globalni tabulky
	// nesmi se shodovat s zadnou promennou v globalni tabulce a s zadnou funkci
	
		tmp = s_table->global->table->ptr[hash_function(name, SIZE)];
		//prohleda prvky v globalni tabulce, diva se po shodnych jmenech u promennych a fci, pokud najde -> SEM_ERROR
		while(tmp!=NULL)
			{
				if(strcasecmp(name, tmp->name) == 0) 
				{
					*error = SEM_ERROR;  //na globalni urovni se nesmi shodovat jmena promennych ani fci
					return NULL;
				}		
				tmp = tmp->next;	
			}
		struct htab_item *newitem = (struct htab_item*) malloc(sizeof(struct htab_item));  //alokace pro novou polozku v tabulce
		if(newitem==NULL){
			*error = INTERNAL_ERR;
			return NULL;
		}
				
		newitem->name = name;
		newitem->global = true;
		newitem->fwd = false;
		newitem->type = s_default;
		newitem->function = false;
		newitem->func_data = NULL;
		newitem->func_table = NULL;
		newitem->initialized = false;
		newitem->index = s_table->global->item_count; //dostane index rovny aktualni hodnote pocitadla
		s_table->global->item_count++; //pocitadlo promenych se zvetsi o 1;
		
		//pripojime strukturu do tabulky
		newitem->next = s_table->global->table->ptr[hash_function(name, SIZE)];
		s_table->global->table->ptr[hash_function(name, SIZE)] = newitem;
	
		//vrati ukazatel (pro dalsi upravy, napr. inicializovanost)
		return newitem;
	
	}
	else {
		 //jedna se o lokalni promennou == do lokalni tabulky
		 //nesmi se shodovat s promenou se jmenem ZADNE fce (takova promena tam jiz stejne bude pro return hodnotu)
		 //nesmi se shodovat s zadnou lokalni promennou
		tmp = s_table->local->table->ptr[hash_function(name, SIZE)]; //ukazatel na lokalni tabulku
		while(tmp!=NULL)
		{
			if(strcasecmp(name,tmp->name) == 0) //kolize jmen fci na lokalni urovni
			{
				*error = SEM_ERROR;  
				return NULL;
			}		
			tmp = tmp->next;	
		}	 
		tmp = s_table->global->table->ptr[hash_function(name, SIZE)]; //ukazatel na globalni tabulku
		while(tmp!=NULL)
		{
			if((strcasecmp(name,tmp->name) == 0) && (tmp->function == true)) //kolize jmen fci na globalni urovni
			{
				*error = SEM_ERROR;  
				return NULL;
			}		
			tmp = tmp->next;	
		}
		struct htab_item *newitem = (struct htab_item*) malloc(sizeof(struct htab_item));  //alokace pro novou polozku v tabulce
		if(newitem==NULL){
			*error = INTERNAL_ERR;
			return NULL;
		}
		

		newitem->name = name;
		newitem->global = false;
		newitem->function = false;
		newitem->fwd = false;
		newitem->type = s_default;
		newitem->func_data = NULL;
		newitem->func_table = NULL;
		newitem->initialized = false;
		newitem->index = s_table->local->item_count; //dostane index rovny aktualni hodnote pocitadla
		s_table->local->item_count++; //pocitadlo promenych se zvetsi o 1;
		
		//pripojime strukturu do tabulky
		newitem->next = s_table->local->table->ptr[hash_function(name, SIZE)];
		s_table->local->table->ptr[hash_function(name, SIZE)] = newitem;
	
		//vrati ukazatel (pro dalsi upravy, napr. inicializovanost)
		return newitem;
	}	
}

//-----------------add_func---------------------------------------------
struct htab_item* add_func(char *name_notupper, struct symbol_table* s_table, int* error){
//podivat se do globalni tabulky, pokud uz tam fce je -> error
//jinak vytvorit novou lokalni promennou se stejnym jmenem //nemuze dojit ke kolizi, protoze tabulka bude prazdna (add_local_table se musi volat tesne pred touhle fci)
//vlozi odkaz na tuhle tabulku
//musi si zapamatovat navesti
	int i = 0;	
	//do tabulky se vsechny promenne ukladaji s nazvem z velkych pismen, jazyk je case insensitive a case insensitive hashovaci funkce by ztracela na rychlosti
	char* name = malloc(sizeof(char)*(strlen(name_notupper)+1));
	if(name == NULL){
		*error = INTERNAL_ERR;
		return NULL;
	}
	while(name_notupper[i]){ //prekopirovani retezce a prevod na upper case
		name[i]=toupper(name_notupper[i]);
		i++;
	}
	name[i]= '\0';
	
	struct htab_item *tmp = s_table->global->table->ptr[hash_function(name, SIZE)]; //ukazatel na globalni tabulku
	while(tmp!=NULL)
	{
		if((strcasecmp(name,tmp->name) == 0)) //kolize jmen fci nebo promennych na globalni urovni
		{
			*error = SEM_ERROR;  
			return NULL;
		}		
		tmp = tmp->next;	
	}

	struct htab_item *newitem = (struct htab_item*) malloc(sizeof(struct htab_item));  //alokace pro novou polozku v tabulce
	if(newitem==NULL){
		*error = INTERNAL_ERR;
		return NULL;
	}
	
	newitem->name = name;
	newitem->global = true;
	newitem->fwd = false;
	newitem->type = s_default;
	newitem->function = true;
	newitem->func_data = NULL;
	newitem->func_table = s_table->local;
	newitem->initialized = false;
	newitem->index = -1; //fce budou mit index -1
	if(add_var(name, s_table, error) == NULL){ //do lokalni tabulky fce je pridana promenna s jejim jmenem == return promenna fce
		*error = INTERNAL_ERR;
		return NULL;
	}
	//kazda fce musi mit ukazatel na sve navesti...dodelat!!
	
	//pripojime strukturu do tabulky
	newitem->next = s_table->global->table->ptr[hash_function(name, SIZE)];
	s_table->global->table->ptr[hash_function(name, SIZE)] = newitem;
	
	//vrati ukazatel (pro dalsi upravy, napr. inicializovanost)
	return newitem;
}

//-----------------search_func---------------------------------------------
struct htab_item* search_func(char *name_notupper, struct symbol_table* s_table, int* error){
	
	int i = 0;	
	//do tabulky se vsechny promenne ukladaji s nazvem z velkych pismen, jazyk je case insensitive a case insensitive hashovaci funkce by ztracela na rychlosti
	char name[strlen(name_notupper)+1];
	while(name_notupper[i]){ //prekopirovani retezce a prevod na upper case
		name[i]=toupper(name_notupper[i]);
		i++;
	}
	name[i]= '\0';
	
	struct htab_item *tmp = s_table->global->table->ptr[hash_function(name, SIZE)]; //ukazatel na globalni tabulku
	while(tmp!=NULL)
	{
		if((strcasecmp(name,tmp->name) == 0) && (tmp->function == true)) //kolize jmen fci nebo promennych na globalni urovni
		{  
			return tmp;
		}		
		tmp = tmp->next;	
	}
	*error = SEM_ERROR;
	return NULL;
}

//-----------------search_var---------------------------------------------
struct htab_item* search_var(char *name_notupper, struct symbol_table* s_table, int* error){
	
	int i = 0;	
	//do tabulky se vsechny promenne ukladaji s nazvem z velkych pismen, jazyk je case insensitive a case insensitive hashovaci funkce by ztracela na rychlosti
	char name[strlen(name_notupper)+1];
	while(name_notupper[i]){ //prekopirovani retezce a prevod na upper case
		name[i]=toupper(name_notupper[i]);
		i++;
	}
	name[i]= '\0';
	
	struct htab_item *tmp = s_table->local->table->ptr[hash_function(name, SIZE)]; //ukazatel na lokalni tabulku
	while(tmp!=NULL)
	{
		if(strcasecmp(name,tmp->name) == 0)  //kolize jmen fci nebo promennych na globalni urovni
		{  
			return tmp;
		}		
		tmp = tmp->next;	
	}
	tmp = s_table->global->table->ptr[hash_function(name, SIZE)]; //pote se divam do globalni tabulky
	while(tmp!=NULL)
	{
		if((strcasecmp(name,tmp->name) == 0) && (tmp->function == false)) //kolize jmen fci nebo promennych na globalni urovni
		{  
			return tmp;
		}		
		tmp = tmp->next;	
	}
	
	//pokud jsem nic nenasel, vratim null == semanticka chyba
	*error = SEM_ERROR;
	return NULL;
}

//-----------------table_free---------------------------------------------
void symbol_table_free(struct symbol_table* s_table){
	s_table->local = NULL;
	struct symbol_table_item * tmp_symbol_table_item = NULL;
	
	while(s_table->global != NULL){
		tmp_symbol_table_item = s_table->global;
		s_table->global = s_table->global->next;
		
		for(int i=0; i < tmp_symbol_table_item->table->htab_size; i++)
		{
			struct htab_item *item = tmp_symbol_table_item->table->ptr[i], *tmp = NULL;
			
			while(item!=NULL)
			{
				tmp = item;
				item = item->next;
				free(tmp->name);
				free(tmp->func_data);
				free(tmp);
			}
			tmp_symbol_table_item->table->ptr[i]=NULL;
		}
		
		free(tmp_symbol_table_item->table);
		tmp_symbol_table_item->table=NULL;
		
		free(tmp_symbol_table_item);
	}
	free(s_table);
}

//-----------------func_defined---------------------------------------------
int funcs_defined(struct symbol_table* s_table){
//konecna kontrola na zaver programu, zda vsechny deklarovane funkce byly i definovane, coz musi byt
	struct htab_t *htab = s_table->global->table;
	
	for(int i = 0; i < htab->htab_size; i++)
	{
		struct htab_item *item = htab->ptr[i];
		
		while(item!=NULL)
		{
			if((item->function) && (!item->initialized))	//pokud je polozka funkce a neni inicializovana (coz nesmi) -> SEM_ERROR
				return SEM_ERROR;
			item = item->next; //na dalsi polozku
		}
	}
	return SEM_OK;
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




/**
* Funkce vraci delku retezce
* @param *s ukazatel na strukturu string
* @return delka retezce
*/

int length(string *s)
{
	return s->length;
}


/**
* Funkce vraci kopii n znaku od pozice i ze stringu s
* @param *s ukazatel na puvodni retezec ze struktury string (string->str)
* @param *dest ukazatel na strukturu string (kvuli funkcim s alokaci), TAM BUDE VYSLEDEK
* @param i od jake pozice kopirujeme
* @param n kolik znaku chceme kopirovat
* @param s_len delka retezce s (string->length)
* @return chyba nebo uspech
*/

int copy(char *s, string *dest, int i, int n, int s_len) 
{
	int alokovano = 0;

	if (n < 1 || i > s_len) /* neni co delat */
	{
		return 0;
	}

	if ((i+n) > (s_len+1))
	{
		n = n-(n+i-s_len)+1; /* orezani retezce, kdyz presahuje hranice */
	}

	char *tmp;

	/* jeste zbyva zkontrolovat jestli nekopirujeme ze stejneho retezce do ktereho taky ukladame */
	if (s == dest->str) /* je to jeden a ten samy retezec */
	{
		if ((tmp = (char *) malloc(sizeof(char) * (strlen(s)+1))) == NULL) /* +1 kvuli '\0' */
			return INTERNAL_ERR;
		strcpy(tmp, s);
		tmp[strlen(s)] = '\0';
		strClear(dest);
		s = tmp;
		alokovano = 1;
	}
	else
	{
		strClear(dest);
	}

	int j = i-1;
	int count = 0;

	while(count < n) /* kopirujeme jen tolik kolik chceme */
	{
 		strAddChar(dest, s[j]);
        j++;
        count++;
	}

	if (alokovano) free(tmp);

	return 0;
}


/**
* Funkce hleda podretezec a vraci jeho index (Boyer-Mooreuv algoritmus, 1. heuristika + doplnky proti zacykleni)
* @param *s ukazatel na puvodni retezec (retezec ze struktury string (string->str))
* @param *search ukazatel na hledany podretezec (retezec ze struktury string (string->str))
* @param s_len delka puvodniho retezce (string->length)
* @param search_len delka hledaneho retezce (string->length)
* @return index nebo chyba
*/

int find(char *s, char *search, int s_len, int search_len)
{
	int ascii = 256; /* podle ascii tabulky */
	int max = 0;
	int return_s_len = s_len;

	if (search_len == 0) /* prazdny retezec se vzdy nachazi na pozici 1 */
	{
		return 1; /* toto neni chyba, je to spravny vysledek - index */
	}
	else if (search_len > s_len)
	{
		return 0; /* nenalezen */
	}
	else /* muzeme zacit s vyhledavanim */
	{
		/* vytvoreni pole - vstupni abecedy */
		/* obsahuje 2 cykly */
		int jumps[ascii]; /* velikost pole 256 prvku (ascii) */
		for (int i = 0; i < ascii; i++)
		{
			jumps[i] = search_len;
		}

		/* ascii hodnoty znaku, ktere jsou v search     */
		/* dostanou prirazenou delku, o kterou muzeme   */
		/* sablonu posunout, kdyz ve vyhledavanem textu */
		/* na dany znak narazime...ostatni znaky maji   */
		/* pro skok povolenou delku search */
		for (int j = 0; j < search_len; j++) 
		{
			jumps[((int)search[j])-1] = search_len-j-1;
		}

		int j, k, tmp; /* j...index textu, k...index vyhledavaneho textu, tmp...optimalizace */

		if (search_len == 1)
		{
			k = 1;
			s_len++;
		}
		else
		{
			k = search_len-1;
		}

		j = k;
		max = j;

		/* jadro funkce */
		/* hlavni cyklus */
		while ((k >= 0) && j < s_len) 
		{
			if (s[j] == search[k]) /* aktualni porovnani znaku */
			{
				/* zkousime dalsi shodny znak */
				k--;
				j--;
			}
			else
			{
				tmp = jumps[(int)(s[j])-1];

				/* kdyz mame nulovy skok && znak napravo od aktualniho se rovna */
				/* poslednimu ze search, tak zvysime max (proti zacykleni)      */
				if (tmp == 0 && (s[j+1] == search[search_len-1])) 
				{
					j = max+1;
					max = j;
				}
				/* podminka proti zacykleni */
				else if (j+tmp == (search_len-1)) 
				{		
					j = j+tmp+1;
					max = j;			  
				}
				/* klasicky skok dopredu o tolik, kolik nam rekne tabulka */
				else
				{
					j = j+tmp; 
					if (j == max)
					{
						max++;
						j = max;
					}
					if (tmp != 0 && max < j) max = j;
				}
				
				if (tmp == 0) j = max+1;

				k = search_len-1; /* nastavime index na zacatek sablony (zprava) */
			}
		}
		/* konec hlavniho cyklu */

		/* vraceni spravneho vysledku */
		if (j >= s_len) /* nenalezen */
		{
			return 0;
		}
		else if (k <= 0)
		{
			return j+2;
		}
		else
		{
			return return_s_len+1;	
		}  		
	}
}


/**
* Funkce seradi retezec dest->str (QuickSort - algoritmus z opory 14-Q)
* @param *dest ukazatel na strukturu string, KDE BUDE VYSLEDEK
*/

void sort(string *dest)
{
	if (dest->length <= 1)
		return; /* neni co radit */
	/* hlavni funkce */
	sort_main(dest->str, 0, (dest->length)-1);
}


/**
* Funkce rozdeluje pole
* @param *A ukazatel na retezec ktery se rozdeluje
* @param left index prvniho prvku
* @param right index posledniho prvku
* @param *i pomocny index (left)
* @param *j pomocny index (right)
*/

void partition(char *A, int left, int right, int *i, int *j)
{
	int PM; /* pseudo-median */
	*i = left;
	*j = right;
	int tmp;
	PM = A[((*i)+(*j))/2];
	do
	{
		/* zleva doprava (index roste) */
		while (A[(*i)] < PM)
		{
			(*i)++;
		} 
		
		/* zprava doleva (index klesa) */
		while (A[(*j)] > PM)
		{
			(*j)--;
		}

		if ((*i) <= (*j))
		{
			/* prohozeni prvku */
			tmp = A[(*i)];
			A[(*i)] = A[(*j)];
			A[(*j)] = tmp;
			(*i)++;
			(*j)--;
		}
	} while ((*i) <= (*j));
}


/**
* Funkce seradi vzestupne znaky
* @param *A ukazatel na retezec ktery se bude radit
* @param left index prvniho prvku
* @param right index posledniho prvku
*/

void sort_main(char *A, int left, int right)
{
	int r = right;
	int l = left;
	int *i = &l;
	int *j = &r;

	/* rozdelovaci funkce */
	partition(A, left, right, i, j);

	/* rekurzivni volani */
	if (left < *j) sort_main(A, left, *j);
	if (*i < right) sort_main(A, *i, right);
}



/******************************** READLN ********************************/

/**
* Funkce nacita ze standardniho vstupu typ integer
* Kostra automatu je prevzata ze scanneru
* @param *err ukazatel na int ve kterem je pripadne error
* @return typ int, nebo chyba STDIN_NUM_ERROR
*/

int readln_int(int *err)
{
	int state = 0;
	int c;
	int vysledek = 0; /* zde bude vysledek */
	long int overflow = 0; /* tady nahravam to same co do i...kdyby i melo pretect/podtect tak to overflow pozna */
	int znamenko = 0; /*  0=kladne  1=zaporne  */

	*err = 0;

	while (1)
	{
		c = getchar(); /* nacitame */

		switch(state)
		{
			case 0:

				if (isspace(c)) /* pocatecni stav - vynechavame bile znaky */
				{
					state = 0;
				}
	            else
	            if(c == '+') /* kladne cislo */
	            {
	                state = 2;
	            }
	            else
	            if(c == '-') /* kladne cislo */
	            {
	                znamenko = 1;
	                state = 2;
	            }
	            else
	            if(c >= '0' && c <= '9') /* jdeme nacitat samotne cislo */
	            {
	                vysledek = c - '0';
	                overflow = c - '0'; /* detekce int overflow */
	                state = 1;
	            }
	            else
	            {
	            	*err = STDIN_NUM_ERROR;
	            	return STDIN_NUM_ERROR;
	            }

	        break;


			case 1:

				if (c >= '0' && c <= '9')
				{
					overflow = overflow * 10 + (c - '0'); /* detekce int overflow */
					if (znamenko == 1)
					{
						if (-overflow < INT_MIN) /* cislo preteklo/podteklo */
						{
							*err = STDIN_NUM_ERROR;
							return STDIN_NUM_ERROR;
						}	
					}
					else
					{
						if (overflow > INT_MAX) /* cislo preteklo/podteklo */
						{
							*err = STDIN_NUM_ERROR;
							return STDIN_NUM_ERROR;
						}	
					}
					
					vysledek = vysledek * 10 + (c - '0');			
				}
				else /* cokoliv jineho nez cislo - nezadouci - ignor */
				{
					if (znamenko == 1)
					{
						return -vysledek;
					}
					else
					{
						return vysledek;
					}
				}

			break;


			case 2: /* znamenko bylo zadano */

				if (c >= '0' && c <= '9')
				{
					overflow = overflow * 10 + (c - '0'); /* detekce int overflow */
					if (znamenko == 1)
					{
						if (-overflow < INT_MIN) /* cislo preteklo/podteklo */
						{
							*err = STDIN_NUM_ERROR;
							return STDIN_NUM_ERROR;
						}	
					}
					else
					{
						if (overflow > INT_MAX) /* cislo preteklo/podteklo */
						{
							*err = STDIN_NUM_ERROR;
							return STDIN_NUM_ERROR;
						}	
					}
					
					vysledek = vysledek * 10 + (c - '0');
					state = 1;			
				}
				else
				{
					*err = STDIN_NUM_ERROR;
					return STDIN_NUM_ERROR;
				}

			break;
		}
	}
	return vysledek;
}


/**
* Funkce nacita ze standardniho vstupu typ real
* Kostra automatu je prevzata ze scanneru
* @param *err ukazatel na int ve kterem je pripadne error
* @return typ real, nebo chyba (STDIN_NUM_ERROR/INTERNAL_ERR)
*/

double readln_real(int *err)
{
	int state = 0;
	int c;
	int obs = 0; /* kontorla, zda ciselne exponentu, nebo cisla obsahuji nejakou hodnotu */
	double vysledek;
	int tmp = 11; /* pocatecni hodnota alokace */
	int *alokovano = &tmp;
	int tmp2 = 0; /* pocatecni delka retezce */
	int *delka = &tmp2;
	*err = 0;

	char *s_vysledek;

	if ((s_vysledek = (char *) malloc(sizeof(char)*(tmp))) == NULL)
	{
		*err = INTERNAL_ERR;
		return INTERNAL_ERR;
	}
	s_vysledek[0] = '\0';
	
	while (1)
	{
		c = getchar(); /* nacitame */

		switch(state)
		{
			case 0:

				if (isspace(c)) /* pocatecni stav - vynechavame bile znaky */
				{
					state = 0;
				}
	            else
	            if (c == '+') /* kladne cislo */
				{
					state = 8;
				}
	            else
	            if (c == '-') /* zaporne cislo */
				{
					if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
	        		state = 8;
				}
	            else
	            if(c >= '0' && c <= '9') /* jdeme nacitat samotne cislo */
	            {
	                if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
	                state = 1;
	            }
	            else
	            {
	            	free(s_vysledek);
	            	*err = STDIN_NUM_ERROR;
	            	return STDIN_NUM_ERROR;
	            }

	        break;


	        case 1:

	        	if (c >= '0' && c <= '9')
	        	{
	        		if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
	        	}
	        	else
                {
                	if(c == '.') /* destinne cislo */
                    {
                        if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        			{
	        				free(s_vysledek);
	        				*err = INTERNAL_ERR;
	        				return INTERNAL_ERR;
	        			}
                        obs = 0;
                        state = 2;
                    }
                    else
                    {
                        if(c == 'e' || c == 'E') /* cislo s exponentem */
                        {
                            if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        				{
	        					free(s_vysledek);
	        					*err = INTERNAL_ERR;
	        					return INTERNAL_ERR;
	        				}
                            obs = 0;
                            state = 3;
                        }
                        else
                        {
                        	free(s_vysledek);
                        	*err = STDIN_NUM_ERROR;
                           	return STDIN_NUM_ERROR;
                        }
                    }
                }

	        break;


	        case 2: /* desetinna cast */

                if(c >= '0' && c <= '9')
                {
                    if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
                    obs = 1; /* desetinna cast musi neco obsahovat */
                }
                else
                {
                    if(obs == 0) /* ukonceni desetinne casti bez obsahu - chyba */
                    {
                    	free(s_vysledek);
                    	*err = STDIN_NUM_ERROR;
                    	return STDIN_NUM_ERROR;
                    }
                    if(c == 'e' || c == 'E') /* desetinne cislo s exp */
                    {
                        if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        			{
	        				free(s_vysledek);
	        				*err = INTERNAL_ERR;
	        				return INTERNAL_ERR;
	        			}
                        obs = 0;
                        state = 4;
                    }
                    else
                    {	
                    	vysledek = strtod(s_vysledek, NULL);
                    	free(s_vysledek);
                    	if (vysledek == -0) vysledek = 0;
                        return vysledek;
                    }
                }

            break;


            case 3: /* pouze X.e^(neco) */

                if (c >= '0' && c <= '9')
                {
                    obs = 1;
                    if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
                }
                else
                {
                    if (c == '-' && obs == 0) /* e^-neco */
                    {
                        if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        			{
	        				free(s_vysledek);
	        				*err = INTERNAL_ERR;
	        				return INTERNAL_ERR;
	        			}
                        obs = 0;
                        state = 5;
                    }
                    else
                    {
	                    if (c == '+' && obs == 0) /* e^+neco */
	                    {
	                        obs = 0;
	                        state = 6; 
	                    }
	                    else
	                    {
	                        if (obs == 0)
	                        {
	                        	free(s_vysledek);
	                        	*err = STDIN_NUM_ERROR;
	                          	return STDIN_NUM_ERROR; 
	                        }
	                        else
							{
	                        	vysledek = strtod(s_vysledek, NULL);
		                    	free(s_vysledek);
		                    	if (vysledek == -0) vysledek = 0;
		                        return vysledek;
	                        }
	                    }
                   	}
               	}

            break;


            case 4: /* desetinne cislo s exp */

            	if(c>='0' && c<='9')
                {
                    if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
                    obs = 1;
                }
                else
                {
                	if(c == '-' && obs == 0) /* zaporny exp */
                    {
                    	obs = 0;
                        if ((s_vysledek = doubleAddChar(s_vysledek, c, alokovano, delka)) == NULL)
	        			{
	        				free(s_vysledek);
	        				*err = INTERNAL_ERR;
	        				return INTERNAL_ERR;
	        			}
                        state = 9;
                    }
                    else
                    {
                    	if(c == '+' && obs == 0)
                        {
                            obs = 0;
                            state = 7;  /* kladny exp nema vliv */
                        }
                        else
                        {
                            if (obs == 0) 
                            {
                            	free(s_vysledek);
                            	*err = STDIN_NUM_ERROR;
                            	return STDIN_NUM_ERROR;
                            }
                           	vysledek = strtod(s_vysledek, NULL);
	                    	free(s_vysledek);
	                    	if (vysledek == -0) vysledek = 0;
	                        return vysledek;
                        }
                   }
               }

            break;


            case 5: /* e^-neco */

            	if(c >= '0' && c <= '9')
                {
                    if (doubleAddChar(s_vysledek, c, alokovano, delka) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
                    obs = 1;
                }
                else
                {
                	if(obs == 0)
                    {
                    	free(s_vysledek);
                    	*err = STDIN_NUM_ERROR;
                    	return STDIN_NUM_ERROR;
                    }
                    vysledek = strtod(s_vysledek, NULL);
                    free(s_vysledek);
                    if (vysledek == -0) vysledek = 0;
                    return vysledek;
                }

            break;


            case 6:

            	if(c >= '0' && c <= '9')
                {
                    if (doubleAddChar(s_vysledek, c, alokovano, delka) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
                    obs = 1;
                }
                else
                {
                    if(obs == 0)
                    {
                    	free(s_vysledek);
                    	*err = STDIN_NUM_ERROR;
                    	return STDIN_NUM_ERROR;
                    }
                    vysledek = strtod(s_vysledek, NULL);
                    free(s_vysledek);
                    if (vysledek == -0) vysledek = 0;
                    return vysledek;
                }

            break;


            case 7:

            	if(c >= '0' && c <= '9')
                {
                    if (doubleAddChar(s_vysledek, c, alokovano, delka) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
                    obs = 1;
                }
                else
                {
                    if(obs == 0)
                    {
                    	free(s_vysledek);
                    	*err = STDIN_NUM_ERROR;
                    	return STDIN_NUM_ERROR;
                    }
                    vysledek = strtod(s_vysledek, NULL);
                    free(s_vysledek);
                    if (vysledek == -0) vysledek = 0;
                    return vysledek;
                }

            break;


            case 8: /* samotne + nebo - */

            	if(c >= '0' && c <= '9')
                {
                    if (doubleAddChar(s_vysledek, c, alokovano, delka) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
                    state = 1;
                }
                else
                {
                	free(s_vysledek);
                    *err = STDIN_NUM_ERROR;
                    return STDIN_NUM_ERROR;
                }


            break;


            case 9:  /* desetinne cislo se zapornym exp */

            	if(c >= '0' && c <= '9')
                {
                    if (doubleAddChar(s_vysledek, c, alokovano, delka) == NULL)
	        		{
	        			free(s_vysledek);
	        			*err = INTERNAL_ERR;
	        			return INTERNAL_ERR;
	        		}
                    obs = 1;
                }
                else
                {
                    if(obs == 0)
                    {
                    	free(s_vysledek);
                    	*err = STDIN_NUM_ERROR;
                    	return STDIN_NUM_ERROR;
                    }
                   	vysledek = strtod(s_vysledek, NULL);
                    free(s_vysledek);
                    if (vysledek == -0) vysledek = 0;
                    return vysledek;
                }

            break;
		}
	}
}


/**
* Funkce nacita ze standardniho vstupu typ string
* @param *err ukazatel na int ve kterem je pripadne error
* @return typ ukazatel na retezec, nebo chyba NULL
*/

char* readln_string(int *err)
{
	int prirustek = 10; /* pomocna promenna pro realokaci */
	int alokovano = prirustek+1; /* pomocna promenna pro realokaci */
	int c;
	int counter = 0;

	char *vysledek;
	if ((vysledek = (char *) malloc(sizeof(char)*(prirustek+1))) == NULL) /* pocatecni velikost 11 */
	{
		*err = INTERNAL_ERR;
		return NULL;
	}

	vysledek[0] = '\0';

	while ((c = getchar()) != EOF) /* nacitame do konce */
	{
		if (c == '\n')
		{
			break;	
		}

		if (counter+1 == alokovano) /* musime alokovat vice mista */
		{
			if ((vysledek = (char *) realloc(vysledek, alokovano+prirustek)) == NULL)
			{
				*err = INTERNAL_ERR;
				return NULL; 
			}
			alokovano = alokovano+prirustek;
		}

		vysledek[counter] = c; /* pridani nacteneho znaku do retezce */
		counter++;
	}
	
	vysledek[counter] = '\0'; /* zakonceni retezce */
	
	return vysledek;
}


/**
* Pomocna funkce (pro readln_real), pro pridavani znaku ke stringu
* @return uspech nebo chyba, ktera probubla dal
*/

char* doubleAddChar(char *s1, char c, int *alokovano, int *delka)
{
	int prirustek = 10;

   	if ((*delka)+1 >= *alokovano)
   	{
    	/* pamet nestaci, je potreba provest realokaci */
      	if ((s1 = (char*) realloc(s1, strlen(s1) + prirustek)) == NULL)
        {
        	return NULL;
        }

      	*alokovano = *delka + prirustek;
   	}
   	s1[*delka] = c;
   	(*delka)++;
   	s1[(*delka)] = '\0';

   	return s1;
}
