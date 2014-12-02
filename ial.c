#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "ial.h"
#include "err.h"


//-----------------hash_function---------------------------------------------
unsigned int hash_function(const char *str, unsigned htab_size)
//hashovaci funkce, ktera pro zadany retezec vraci hodnotu z pozadovaneho intervalu
{
 unsigned int h=0;
 const unsigned char *p;
 for(p=(const unsigned char*)str; *p!='\0'; p++)
 		h = 65599*h + *p;
 return h % htab_size;
}
 

/************************************************/
/* inicializace a alokace nove tabulky symbolu	*/
/* 						*/
/************************************************/
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


/************************************************/
/* alokuje novou lokalni tabulku		*/
/* 						*/
/************************************************/
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

/************************************************/
/* "zappomene" na aktualni lokalni tabulku      */
/* a ukazatel se vrati zpatky na globaln	*/
/************************************************/
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





/************************************************/
/* vraci delku stringu	                        */
/* v interpretu radeji rovnou strlen (rychlost) */
/************************************************/

int length(char *s)
{
	return strlen(s);
}


/************************************************/
/* vraci kopii n znaku od pozice i ze stringu s */
/************************************************/

char *copy(char *s, int i, int n) 
{
	if (s == NULL || i < 1 || n < 1 || i > strlen(s))
	{
		return SEM_ERROR;
	}

	if ((i+n) > (strlen(s)+1))
	{
		n = n-(n+i-strlen(s))+1; /* orezani retezce, kdyz presahuje hranice */
	}

	/* vyresit alokaci a praci s ukazatelem!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	char *ret = (char *) malloc(sizeof(char) * (n+1)); /* +1 kvuli '\0' */

	int j = i-1;
	int count = 0;

	while(count < n)
	{
 		ret[count] = s[j];
        j++;
        count++;
	}
	ret[n] = '\0';

	return ret;
}


/**********************************************/
/* vraci pozici podretezce search z retezce s */
/**********************************************/

int find(char *s, char *search)
{
	int search_len = strlen(search);
	int s_len = strlen(s);
	int ascii = 256; /* podle ascii tabulky */
	int max = 0;

	if (s == NULL || search == NULL) /* neplatne ukazatele */
	{
		return SEM_ERROR;
	}
	else if (search_len == 0) /* prazdny retezec se vzdy nachazi na pozici 1 */
	{
		return 1;
	}
	else if (search_len > s_len)
	{
		return 0;
	}
	else /* konecne muzeme zacit s vyhledavanim */
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
				/* uz nevim co to je, ale nemam odvahu to smazat */
				else if (j+tmp == (search_len-1)) 
				{		
					j = j+tmp+1;
					max = j;			  
				}
				/* klasicky skok dopredu o tolik, kolik nam rekne tabulka */
				else
				{
					j = j+tmp; 
					if (tmp != 0) max = j;
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
			return strlen(s)+1;	
		}  		
	}
}


/**********************************************/
/* seradi vstupni retezec A vzestupne a vraci */ 
/* novy retezec (ten vstupni se nemeni)       */
/**********************************************/

/* vyresit malloc...kdyz si vytvorim vstupni retezec */
/* a namallocuju si misto pro jeho serazenou kopii   */
/* a spustim nad tim tuto funkci tak funguje!        */
void sort(char *A, int left, int right) /* zmenit navratovou hodnotu */
{
	int r = right;
	int l = left;
	int *i = &l;
	int *j = &r;

	partition(A, left, right, i, j);

	if (left < *j) sort(A, left, *j);
	if (*i < right) sort(A, *i, right);
}


/***************************************/
/* pomocna rozdelovaci funkce pro sort */
/***************************************/

void partition(char *A, int left, int right, int *i, int *j)
{
	int PM; /* pseudo-median */
	*i = left;
	*j = right;
	int tmp;
	PM = A[((*i)+(*j))/2];
	//printf("PM: %d\n", PM);
	do
	{
		//printf("i: %d; j: %d\n", *i, *j);
		while (A[(*i)] < PM)
		{
			//printf("cyklus a\n");
			(*i)++;
		} 

		while (A[(*j)] > PM)
		{
			//printf("cyklus b\n");
			(*j)--;
		}

		if ((*i) <= (*j))
		{
			//printf("swap\n");
			tmp = A[(*i)];
			A[(*i)] = A[(*j)];
			A[(*j)] = tmp;
			(*i)++;
			(*j)--;
		}
	} while ((*i) <= (*j));
}




