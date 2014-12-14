#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "str.h"
#include "ial.h"
#include "list.h"
#include "interpret.h"
#include "inter-stacks.h"
#include "printlist.h"
#include "err.h"
#include "garbage.h"


#define VAR_STACK_SIZE 50
#define ADS_STACK_SIZE 50
#define LVS_STACK_SIZE 50

/* Inicializace ramce hodnot */
void initarray(tVarArr *array, int size)
{
	for (int i = 0; i < size; i++)
		array[i].var = NULL;
}

/* Nacteni ramce hodnot
	Parametry jsou pole tvorici ramec a ukazatel na tabulku symbolu patrici k danemu ramci.
	Funkce prochazi celou tabulku symbolu a podle indexu a typu promennych pozna kam a co
	v ramci naalokovat. Zaroven nastavuje hodnotu inicializace na 0. */
int loadarray(tVarArr *array, symbol_table_item *TB)
{
	int hash_size = TB->table->htab_size;
	htab_item *iptr = NULL;

	for (int i = 0; i < hash_size; i++)
	{
		iptr = TB->table->ptr[i];
		while (iptr != NULL)
		{
			if (!(iptr->function))
			{
				int type = iptr->type;
				array[iptr->index].init = 0;
				switch (type)
				{
					case 0:
					{
						array[iptr->index].var = malloc(sizeof(int));
						if (array[iptr->index].var == NULL)
							return INTERNAL_ERR;
						break;
					}

					case 1:
					{
						array[iptr->index].var = malloc(sizeof(double));
						if (array[iptr->index].var == NULL)
							return INTERNAL_ERR;
						break;
					}

					case 2:
					{
						array[iptr->index].var = malloc(sizeof(char));
						if (array[iptr->index].var == NULL)
							return INTERNAL_ERR;
						((char*) array[iptr->index].var)[0] = '\0';
						break;
					}

					case 3:
					{
						array[iptr->index].var = malloc(sizeof(bool));
						if (array[iptr->index].var == NULL)
							return INTERNAL_ERR;
						break;
					}

					default:
						break;
				}
			}
			iptr = iptr->next;
		}
	}
	return 0;
}

/* Uvolneni ramce hodnot */
void disposearray(tVarArr *array, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (array[i].var != NULL)
			free(array[i].var);
	}

	if (array != NULL)
		free(array);
}


/* Hlavni funkce interpretu
	Parametry jsou ukazatel na globalni tabulku symbolu a ukazatel na instrukcni list.
	Pouzite konstanty:	INTERNAL_ERR - navratova chyba 99, chyba pri alokaci
						RUN_INIT_ERR - navratova chyba 7, prace s neinicializovanou hodnotou
						RUN_ZERO_ERR - navratova chyba 8, deleni nulou */
int interpret(symbol_table_item *GTable, tList *List)
{
	garbage_list* Garbage = garbage_init();
	
	First(List);
	if (List == NULL)
		return 0;
	tInstr *I;
	string *SPtr = NULL;
	tPrintList *PrintList = malloc(sizeof(tPrintList));
	if (PrintList == NULL)
		return INTERNAL_ERR;
	InitPrintList(PrintList);
	char *str_temp = NULL;
	int *index_temp = malloc(sizeof(int));
	if (index_temp == NULL)
		return INTERNAL_ERR;
	int *size_temp = malloc(sizeof(int));
	if (size_temp == NULL)
		return INTERNAL_ERR;
	int *type_temp = malloc(sizeof(int));
	if (type_temp == NULL)
		return INTERNAL_ERR;
	int index1 = -1;
	int index2 = -1;
	int index3 = -1;
	int type1 = -1;
	int type2 = -1;
	int type3 = -1;
	int i = 0;
	bool *init_temp = malloc(sizeof(bool));
	if (init_temp == NULL)
		return INTERNAL_ERR;
	bool *scope_temp = malloc(sizeof(bool));
	if (scope_temp == NULL)
		return INTERNAL_ERR;
	bool scope1 = -1; //pro scope plati: 0 == local, 1 == global
	bool scope2 = -1;
	bool scope3 = -1;
	void *value_temp = NULL;
	void *return_addr = NULL;
	void *var1 = NULL;
	void *var2 = NULL;
	void *var3 = NULL;
	tVarS *VS = malloc(sizeof(tVarS)); //variable stack
	if (VS == NULL)
		return INTERNAL_ERR;
	VS->var_stack = malloc(sizeof(void*) * (VAR_STACK_SIZE));
	if (VS->var_stack == NULL)
		return INTERNAL_ERR;
	VarStackInit(VS,VAR_STACK_SIZE);
	tVarArr *g_arr = malloc(sizeof(tVarArr) * GTable->item_count);
	if (g_arr == NULL)
		return INTERNAL_ERR;
	initarray(g_arr, GTable->item_count);
	if ((loadarray(g_arr, GTable)) == INTERNAL_ERR)
		return INTERNAL_ERR;
	tVarArr *l_arr = NULL;

	tLVS *LS = malloc(sizeof(tLVS)); //local array stack
	if (LS == NULL)
		return INTERNAL_ERR;
	LS->l_stack = malloc(sizeof(tVarArr) * (LVS_STACK_SIZE));
	if (LS->l_stack == NULL)
		return INTERNAL_ERR;
	LStackInit(LS, LVS_STACK_SIZE);

	tAddS *AS = malloc(sizeof(tAddS)); //return address stack
	if (AS == NULL)
		return INTERNAL_ERR;
	AS->add_stack = malloc(sizeof(tItem)*(ADS_STACK_SIZE));
	if (AS->add_stack == NULL)
		return INTERNAL_ERR;
	AddStackInit(AS, ADS_STACK_SIZE);

	while (1)
	{
		I = GetData(List);
		switch (I->Type)
		{
			//pomocne instrukce

			/* Slouzi pouze jako navesti v instrukcnim listu. */
			case I_LABEL:
				break;

			/* I->addr1 = adresa na promennou v tabulce symbolu
			   I->addr3 = adresa na polozku v instrukcnim listu
			   Ziska si potrebne informace o promenne, ve ktere je ulozena vysledna hodnota podminky.
			   Nasledne zkontroluje, zda byla promenna inicializovana a nakonec vyhodnoti, zda by se melo
			   nebo nemelo skocit v seznamu instrukci na zadanou adresu. */
			case I_IFGOTO:
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				if (scope1 == 0)
				{
					if (l_arr[index1].init == 0)
						return RUN_INIT_ERROR;
				}
				else
				{
					if (g_arr[index1].init == 0)
						return RUN_INIT_ERROR;
				}
				if (type1 != 2)
				{
					if (scope1 == 0)
					{
						if (!(*((bool*) l_arr[index1].var)))
							GoToItem(List,((tItem*) I->addr3));
					}
					else
					{
						if (!(*((bool*) g_arr[index1].var)))
							GoToItem(List,((tItem*) I->addr3));
					}
				}
				break;
			}

			   /*I->addr3 = adresa na polozku v instrukcnim listu
			     Skoci v seznamu instrukci na zadanou adresu. */
			case I_GOTO:
			{
				GoToItem(List,((tItem*) I->addr3));
				break;
			}

			/* Uvolni docasne promenne, zasobniky a globalni tabulku symbolu a zavola Garbage Collector. */
			case I_END:
			{
				
				free(index_temp);
				free(size_temp);
				free(type_temp);
				free(scope_temp);
				free(init_temp);
				DisposeVarStack(VS);
				free(VS->var_stack);
				free(VS);
				free(LS->l_stack);
				free(LS);
				free(AS->add_stack);
				free(AS);
				free(PrintList);
				disposearray(g_arr, GTable->item_count);
				garbage_free(Garbage);
				return 0;
			}			

			//instrukce nacteni a zapsani

			/* I->addr3 - adresa promenne v tabulce symbolu
			   Ziska si potrebne informace o promenne, do ktere se ulozi nactena hodnota ze vstupu.
			   Nasledne nastavi inicializovanost promenne na 1 a nakonec zavola podpurnou funkci pro potrebny typ,
			   ktera vrati hodnotu nactenou ze vstupu a ta se ulozi do ramce hodnot. */
			case I_READ:
			{
				int errcheck = 0;
				char *temp = NULL;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				switch (type3)
				{
					case 0:
					{
						if (scope3 == 0)
							*((int*) l_arr[index3].var) = readln_int(&errcheck);
						else
							*((int*) g_arr[index3].var) = readln_int(&errcheck);
						if (errcheck != 0)
							return errcheck;
						break;
					}
					case 1:
					{
						if (scope3 == 0)
							*((double*) l_arr[index3].var) = readln_real(&errcheck);
						else
							*((double*) g_arr[index3].var) = readln_real(&errcheck);
						if (errcheck != 0)
							return errcheck;
						break;
					}
					case 2:
					{
						temp = readln_string(&errcheck);
						if (errcheck != 0)
							return errcheck;
						garbage_add(Garbage, temp);
						if (scope3 == 0)
						{
							if (strlen(temp) != strlen(((char*) l_arr[index3].var)))
							{
								free(l_arr[index3].var);
								l_arr[index3].var = NULL;
								l_arr[index3].var = malloc(sizeof(char) * (strlen(temp) + 1));
								if (l_arr[index3].var == NULL)
									return INTERNAL_ERR;
							}
							memcpy(((char*) l_arr[index3].var), temp, (strlen(temp) + 1) * sizeof(char));
						}
						else
						{
							if (strlen(temp) != strlen(((char*) g_arr[index3].var)))
							{
								free(g_arr[index3].var);
								g_arr[index3].var = NULL;
								g_arr[index3].var = malloc(sizeof(char) * (strlen(temp) + 1));
								if (g_arr[index3].var == NULL)
									return INTERNAL_ERR;
							}
							memcpy(((char*) g_arr[index3].var), temp, (strlen(temp) + 1) * sizeof(char));
						}
						break;
					}
				}
				break;
			}

			/* I->addr3 - retezec slouzici jako seznam parametru
			   Postupne nacita ze zasobniku hodnotu inicializace parametru a hodnotu parametru. Pokud byl
			   parametr inicializovany pred zavolanim funkce write(), ulozi se jeho hodnota na list tisknuti.
			   Tento list se nakonec vytiskne v obracenem poradi. */
			case I_WRITE:
			{
				if (I->addr3 != NULL)				
					*size_temp = strlen(((char*) I->addr3));
				else
					*size_temp = 0;
				if ((garbage_add(Garbage, I->addr3)) == 1)
					return INTERNAL_ERR; 

				for (int i = (*size_temp - 1); i >= 0; i--)
				{
					if ((BoolVarStackPop(VS)) == 0)
						return RUN_INIT_ERROR;
					switch (((char*) I->addr3)[i])
					{
						case 'i':
						{
							value_temp = malloc(sizeof(int));
							if (value_temp == NULL)
								return INTERNAL_ERR;
							*((int*) value_temp) = IntVarStackPop(VS);
							if ((InsertPrintNew(PrintList, 0, ((void*) value_temp))) == INTERNAL_ERR)
								return INTERNAL_ERR;
							if ((garbage_add(Garbage, value_temp)) == 1)
								return INTERNAL_ERR; 
							value_temp = NULL;
							break;
						}

						case 'r':
						{
							value_temp = malloc(sizeof(double));
							if (value_temp == NULL)
								return INTERNAL_ERR;
							*((double*) value_temp) = DoubleVarStackPop(VS);
							if ((InsertPrintNew(PrintList, 1, ((void*) value_temp))) == INTERNAL_ERR)
								return INTERNAL_ERR;
							if ((garbage_add(Garbage, value_temp)) == 1)
								return INTERNAL_ERR; 
							value_temp = NULL;
							break;
						}
			
						case 's':
						{
							if ((InsertPrintNew(PrintList, 2, ((void*) StrVarStackPop(VS)))) == INTERNAL_ERR)
								return INTERNAL_ERR;
							break;
						}

						case 'b':
						{
							value_temp = malloc(sizeof(bool));
							if (value_temp == NULL)
								return INTERNAL_ERR;
							*((bool*) value_temp) = BoolVarStackPop(VS);
							if ((InsertPrintNew(PrintList, 3, ((void*) value_temp))) == INTERNAL_ERR)
								return INTERNAL_ERR;
							if ((garbage_add(Garbage, value_temp)) == 1)
								return INTERNAL_ERR; 
							value_temp = NULL;
							break;
						}
					}
							
				}
				PrintAll(PrintList);
				break;
			}

			//vestavene funkce

			/* I->addr1 - adresa tabulky symbolu pro funkci find()
			   I->addr3 - adresa promenne v tabulce symbolu
			   Ziska si potrebne informace o promenne, do ktere se ulozi navratova hodnota.
			   Nasledne, pokud existuje, vlozi na zasobnik lokalnich ramcu momentalni ramec.
			   Pote si nacte novy lokalni ramec, odpovidajici tabulce symbolu funkce find().
			   V dalsich krocich postupne zkontroluje inicializovanost parametru a nacte jejich hodnoty ze zasobniku.
			   Nakonec zavola podpurnou funkci find() a jeji navratovou hodnotu si ulozi do docasne promenne. Pak uvolni momentalni
			   lokalni ramec, nacte si nadrazeny, pokud existuje, a ulozi navratovou hodnotu k dane promenne. Zaroven
			   taky teto promenne nastavi inicializovanost na 1. */
			case I_FIND:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;

				if (l_arr != NULL)
					if ((LStackPush(LS, l_arr)) == INTERNAL_ERR)
						return INTERNAL_ERR;

				l_arr = NULL;
				l_arr = malloc(sizeof(tVarArr) * (((htab_item*) I->addr1)->func_table->item_count));
				if (l_arr == NULL)
					return INTERNAL_ERR;
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				if ((loadarray(l_arr, ((htab_item*) I->addr1)->func_table)) == INTERNAL_ERR)
					return INTERNAL_ERR;

				if ((BoolVarStackPop(VS)) == 0)
					return RUN_INIT_ERROR;

				str_temp = StrVarStackPop(VS);
				if (strlen(str_temp) != strlen((char*) l_arr[2].var))
				{
					free(l_arr[2].var);
					l_arr[2].var = NULL;
					l_arr[2].var = malloc(sizeof(char) * (strlen(str_temp) + 1));
					if (l_arr[2].var == NULL)
						return INTERNAL_ERR;
				}
				memcpy(((char*) l_arr[2].var), str_temp, sizeof(char) * (strlen(str_temp) + 1));

				if ((BoolVarStackPop(VS)) == 0)
					return RUN_INIT_ERROR;

				str_temp = StrVarStackPop(VS);
				if (strlen(str_temp) != strlen((char*) l_arr[1].var))
				{
					free(l_arr[1].var);
					l_arr[1].var = NULL;
					l_arr[1].var = malloc(sizeof(char) * (strlen(str_temp) + 1));
					if (l_arr[1].var == NULL)
						return INTERNAL_ERR;
				}
				memcpy(((char*) l_arr[1].var), str_temp, sizeof(char) * (strlen(str_temp) + 1));
				value_temp = malloc(sizeof(int));
				if (value_temp == NULL)
					return INTERNAL_ERR;
				*((int*) value_temp) = find(((char*) l_arr[1].var), ((char*) l_arr[2].var), strlen((char*) l_arr[1].var), strlen((char*) l_arr[2].var));

				disposearray(l_arr, (*size_temp));
				l_arr = NULL;
				if (!(LStackEmpty(LS)))
					l_arr = LStackPop(LS);

				if (*scope_temp == 0)
				{
					*((int*) l_arr[*index_temp].var) = *((int*) value_temp);
					l_arr[*index_temp].init = 1;
				}
				else
				{
					*((int*) g_arr[*index_temp].var) = *((int*) value_temp);
					g_arr[*index_temp].init = 1;
				}

				free(value_temp);
				break;
			}

			/* I->addr1 - adresa tabulky symbolu pro funkci sort()
			   I->addr3 - adresa promenne v tabulce symbolu
			   Ziska si potrebne informace o promenne, do ktere se ulozi navratova hodnota.
			   Nasledne, pokud existuje, vlozi na zasobnik lokalnich ramcu momentalni ramec.
			   Pote si nacte novy lokalni ramec, odpovidajici tabulce symbolu funkce sort().
			   V dalsich krocich postupne zkontroluje inicializovanost parametru a nacte jeho hodnotu ze zasobniku.
			   Nakonec zavola podpurnou funkci sort() a ulozi serazeny retezec do docasne promenne. Pak uvolni momentalni
			   lokalni ramec, nacte si nadrazeny, pokud existuje, a aktualizuje hodnotu serazovane promenne. */
			case I_SORT:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;

				if (l_arr != NULL)
					if ((LStackPush(LS, l_arr)) == INTERNAL_ERR)
						return INTERNAL_ERR;

				l_arr = NULL;
				l_arr = malloc(sizeof(tVarArr) * (((htab_item*) I->addr1)->func_table->item_count));
				if (l_arr == NULL)
					return INTERNAL_ERR;
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				if ((loadarray(l_arr, ((htab_item*) I->addr1)->func_table)) == INTERNAL_ERR)
					return INTERNAL_ERR;

				if ((BoolVarStackPop(VS)) == 0)
					return RUN_INIT_ERROR;

				str_temp = StrVarStackPop(VS);
				
				if (strlen(str_temp) != strlen((char*) l_arr[0].var))
				{
					free(l_arr[0].var);
					l_arr[0].var = NULL;
					l_arr[0].var = malloc(sizeof(char) * (strlen(str_temp) + 1));
					if (l_arr[0].var == NULL)
						return INTERNAL_ERR;
				}
				memcpy(((char*) l_arr[0].var), str_temp, sizeof(char) * (strlen(str_temp) + 1));
				SPtr = malloc(sizeof(string));
				if (SPtr == NULL)
					return INTERNAL_ERR;
				SPtr->str = ((char*) l_arr[0].var);
				SPtr->length = strlen((char*) l_arr[0].var);
				SPtr->allocSize = SPtr->length + 1;
				
				sort(SPtr);
				value_temp = malloc(sizeof(char) * (strlen((char*) l_arr[0].var) + 1));
				if (value_temp == NULL)
					return INTERNAL_ERR;
				memcpy(((char*) value_temp), ((char*) l_arr[0].var), sizeof(char) * (strlen((char*) l_arr[0].var) + 1));

				disposearray(l_arr, (*size_temp));
				l_arr = NULL;
				if (!(LStackEmpty(LS)))
					l_arr = LStackPop(LS);

				if (*scope_temp == 0)
				{
					if (strlen((char*) l_arr[*index_temp].var) != strlen((char*) value_temp))
					{
						free(l_arr[*index_temp].var);
						l_arr[*index_temp].var = NULL;
						l_arr[*index_temp].var = malloc(sizeof(char) * (strlen(value_temp) + 1));
						if (l_arr[*index_temp].var == NULL)
							return INTERNAL_ERR;
					}
					memcpy(((char*) l_arr[*index_temp].var), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
					l_arr[*index_temp].init = 1;
				}
				else
				{
					if (strlen((char*) g_arr[*index_temp].var) != strlen((char*) value_temp))
					{
						free(g_arr[*index_temp].var);
						g_arr[*index_temp].var = NULL;
						g_arr[*index_temp].var = malloc(sizeof(char) * (strlen(value_temp) + 1));
						if (g_arr[*index_temp].var == NULL)
							return INTERNAL_ERR;
					}
					memcpy(((char*) g_arr[*index_temp].var), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
					g_arr[*index_temp].init = 1;
				}

				free(value_temp);
				free(SPtr);
				SPtr = NULL;
				break;
			}

			/* I->addr1 - adresa tabulky symbolu pro funkci length()
			   I->addr3 - adresa promenne v tabulce symbolu
			   Ziska si potrebne informace o promenne, do ktere se ulozi navratova hodnota.
			   Nasledne, pokud existuje, vlozi na zasobnik lokalnich ramcu momentalni ramec.
			   Pote si nacte novy lokalni ramec, odpovidajici tabulce symbolu funkce length().
			   V dalsich krocich postupne zkontroluje inicializovanost parametru a nacte jejo hodnotu ze zasobniku.
			   Nakonec zavola podpurnou funkci length() a jeji navratovou hodnotu si ulozi do docasne promenne. Pak uvolni momentalni
			   lokalni ramec, nacte si nadrazeny, pokud existuje, a ulozi navratovou hodnotu k dane promenne. Zaroven
			   taky teto promenne nastavi inicializovanost na 1. */
			case I_LENGTH:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;

				if (l_arr != NULL)
					if ((LStackPush(LS, l_arr)) == INTERNAL_ERR)
						return INTERNAL_ERR;

				l_arr = NULL;
				l_arr = malloc(sizeof(tVarArr) * (((htab_item*) I->addr1)->func_table->item_count));
				if (l_arr == NULL)
					return INTERNAL_ERR;
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				if ((loadarray(l_arr, ((htab_item*) I->addr1)->func_table)) == INTERNAL_ERR)
					return INTERNAL_ERR;

				if ((BoolVarStackPop(VS)) == 0)
					return RUN_INIT_ERROR;

				str_temp = StrVarStackPop(VS);
				
				if (strlen(str_temp) != strlen((char*) l_arr[1].var))
				{
					free(l_arr[1].var);
					l_arr[1].var = NULL;
					l_arr[1].var = malloc(sizeof(char) * (strlen(str_temp) + 1));
					if (l_arr[1].var == NULL)
						return INTERNAL_ERR;
				}
				memcpy(((char*) l_arr[1].var), str_temp, sizeof(char) * (strlen(str_temp) + 1));
				value_temp = malloc(sizeof(int));
				if (value_temp == NULL)
					return INTERNAL_ERR;

				SPtr = malloc(sizeof(string));
				if (SPtr == NULL)
					return INTERNAL_ERR;
				SPtr->str = ((char*) l_arr[1].var);
				SPtr->length = strlen((char*) l_arr[1].var);
				SPtr->allocSize = SPtr->length + 1;

				*((int*) value_temp) = length(SPtr);

				disposearray(l_arr, (*size_temp));
				l_arr = NULL;
				if (!(LStackEmpty(LS)))
					l_arr = LStackPop(LS);

				if (*scope_temp == 0)
				{
					*((int*) l_arr[*index_temp].var) = *((int*) value_temp);
					l_arr[*index_temp].init = 1;
				}
				else
				{
					*((int*) g_arr[*index_temp].var) = *((int*) value_temp);
					g_arr[*index_temp].init = 1;
				}

				free(value_temp);
				free(SPtr);
				SPtr = NULL;
				break;
			}


			/* I->addr1 - adresa tabulky symbolu pro funkci copy()
			   I->addr3 - adresa promenne v tabulce symbolu
			   Ziska si potrebne informace o promenne, do ktere se ulozi navratova hodnota.
			   Nasledne, pokud existuje, vlozi na zasobnik lokalnich ramcu momentalni ramec.
			   Pote si nacte novy lokalni ramec, odpovidajici tabulce symbolu funkce copy().
			   V dalsich krocich postupne zkontroluje inicializovanost parametru a nacte jejich hodnoty ze zasobniku.
			   Nakonec zavola podpurnou funkci copy() a jeji navratovou hodnotu si ulozi do docasne promenne. Pak uvolni momentalni
			   lokalni ramec, nacte si nadrazeny, pokud existuje, a ulozi navratovou hodnotu k dane promenne. Zaroven
			   taky teto promenne nastavi inicializovanost na 1. */
			case I_COPY:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;

				if (l_arr != NULL)
					if ((LStackPush(LS, l_arr)) == INTERNAL_ERR)
						return INTERNAL_ERR;

				l_arr = NULL;
				l_arr = malloc(sizeof(tVarArr) * (((htab_item*) I->addr1)->func_table->item_count));
				if (l_arr == NULL)
					return INTERNAL_ERR;
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				if ((loadarray(l_arr, ((htab_item*) I->addr1)->func_table)) == INTERNAL_ERR)
					return INTERNAL_ERR;

				if ((BoolVarStackPop(VS)) == 0)
					return RUN_INIT_ERROR;

				*((int*) l_arr[3].var) = IntVarStackPop(VS);

				if ((BoolVarStackPop(VS)) == 0)
					return RUN_INIT_ERROR;

				*((int*) l_arr[2].var) = IntVarStackPop(VS);

				if (*((int*) l_arr[2].var) < 1)
					return RUN_OTHER_ERROR;

				if ((BoolVarStackPop(VS)) == 0)
					return RUN_INIT_ERROR;

				str_temp = StrVarStackPop(VS);
				if (strlen(str_temp) != strlen((char*) l_arr[1].var))
				{
					free(l_arr[1].var);
					l_arr[1].var = NULL;
					l_arr[1].var = malloc(sizeof(char) * (strlen(str_temp) + 1));
					if (l_arr[1].var == NULL)
						return INTERNAL_ERR;
				}
				memcpy(((char*) l_arr[1].var), str_temp, sizeof(char) * (strlen(str_temp) + 1));

				if (strlen(str_temp) != strlen((char*) l_arr[0].var))
				{
					free(l_arr[0].var);
					l_arr[0].var = NULL;
					l_arr[0].var = malloc(sizeof(char) * (strlen(str_temp) + 1));
					if (l_arr[0].var == NULL)
						return INTERNAL_ERR;
				}
				memcpy(((char*) l_arr[0].var), str_temp, sizeof(char) * (strlen(str_temp) + 1));

				SPtr = malloc(sizeof(string));
				if (SPtr == NULL)
					return INTERNAL_ERR;
				SPtr->str = ((char*) l_arr[0].var);
				SPtr->length = strlen((char*) l_arr[0].var);
				SPtr->allocSize = SPtr->length + 1;
				value_temp = malloc(sizeof(int));
				if (value_temp == NULL)
					return INTERNAL_ERR;

				*((int*) value_temp) = copy(((char*) l_arr[1].var), SPtr, *((int*) l_arr[2].var), *((int*) l_arr[3].var), strlen((char*) l_arr[1].var));

				if (*((int*) value_temp) == INTERNAL_ERR)
					return INTERNAL_ERR;

				free(value_temp);
				value_temp = NULL;
				value_temp = malloc(sizeof(char) * (strlen((char*) l_arr[0].var) + 1));
				if (value_temp == NULL)
					return INTERNAL_ERR;

				memcpy(((char*) value_temp), ((char*) l_arr[0].var), sizeof(char) * (strlen((char*) l_arr[0].var) + 1));

				
				disposearray(l_arr, (*size_temp));
				l_arr = NULL;
				if (!(LStackEmpty(LS)))
					l_arr = LStackPop(LS);

				if (*scope_temp == 0)
				{
					if (strlen((char*) l_arr[*index_temp].var) != strlen((char*) value_temp))
					{
						free(l_arr[*index_temp].var);
						l_arr[*index_temp].var = NULL;
						l_arr[*index_temp].var = malloc(sizeof(char) * (strlen(value_temp) + 1));
						if (l_arr[*index_temp].var == NULL)
							return INTERNAL_ERR;
					}
					memcpy(((char*) l_arr[*index_temp].var), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
					l_arr[*index_temp].init = 1;
				}
				else
				{
					if (strlen((char*) g_arr[*index_temp].var) != strlen((char*) value_temp))
					{
						free(g_arr[*index_temp].var);
						g_arr[*index_temp].var = NULL;
						g_arr[*index_temp].var = malloc(sizeof(char) * (strlen(value_temp) + 1));
						if (g_arr[*index_temp].var == NULL)
							return INTERNAL_ERR;
					}
					memcpy(((char*) g_arr[*index_temp].var), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
					g_arr[*index_temp].init = 1;
				}

				free(value_temp);
				free(SPtr);
				SPtr = NULL;
				break;
			}

			//aritmeticke instrukce

			/* Tento popis se vztahuje ke vsem aritmetickym a relacnim operacim.
			   I->addr1 - adresa na promennou do tabulky symbolu, 1. operand
			   I->addr2 - adresa na promennou do tabulky symbolu, 2. operand
			   I->addr3 - adresa na promennou do tabulky symbolu, promenna, kam se ulozi vysleden operace 
			   Nejprve si nacte potrebne informace o promennych a zkontroluje inicializovanost operandu.
			   Nasledne provede potrebnou operaci a hodnotu inicializovanosti vysledkove promenne nastavi na 1. */

			case I_ADD: //+
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				if ((type1 != 2) && (type2 != 2) && (type3 != 2))
				{
					if (scope1 == 0)
						var1 = l_arr[index1].var;
					else
						var1 = g_arr[index1].var;

					if (scope2 == 0)
						var2 = l_arr[index2].var;
					else
						var2 = g_arr[index2].var;

					if (scope3 == 0)
						var3 = l_arr[index3].var;
					else
						var3 = g_arr[index3].var;

					if (type1 == 0)
					{
						if (type2 == 0)
							*((int*) var3) = *((int*) var1) + *((int*) var2);
						else
							*((double*) var3) = ((double)*((int*) var1)) + *((double*) var2);
					}
					else 
					{
						if (type1 == 1)
						{
							if (type2 == 0)
								*((double*) var3) = *((double*) var1) + ((double)(*((int*) var2)));
							else
								*((double*) var3) = *((double*) var1) + *((double*) var2);
						}
					}
				}
				else if ((type1 == 2) && (type2 == 2) && (type3 == 2))
				{
					if (scope1 == 0)
					{
						var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
						if (var1 == NULL)
							return INTERNAL_ERR;
						memcpy(((char*) var1), ((char*) l_arr[index1].var), strlen(((char*) l_arr[index1].var)) + 1);
					}
					else
					{
						var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
						if (var1 == NULL)
							return INTERNAL_ERR;
						memcpy(((char*) var1), ((char*) g_arr[index1].var), strlen(((char*) g_arr[index1].var)) + 1);
					}

					if (scope2 == 0)
					{
						var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
						if (var2 == NULL)
							return INTERNAL_ERR;
						memcpy(((char*) var2), ((char*) l_arr[index2].var), strlen(((char*) l_arr[index2].var)) + 1);
					}
					else
					{
						var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
						if (var2 == NULL)
							return INTERNAL_ERR;
						memcpy(((char*) var2), ((char*) g_arr[index2].var), strlen(((char*) g_arr[index2].var)) + 1);
					}

					var3 = malloc(sizeof(char) * (strlen(var1) + strlen(var2) + 1));
					if (var3 == NULL)
						return INTERNAL_ERR;
					memcpy(((char*) var3), ((char*) var1), (strlen((char*) var1) + 1) * sizeof(char));
					strncat(((char*) var3), ((char*) var2), strlen(((char*) var2)));

					if (scope3 == 0)
					{
						if (strlen(((char*) var3)) != strlen(((char*) l_arr[index3].var)))
						{
							free(l_arr[index3].var);
							l_arr[index3].var = NULL;
							l_arr[index3].var = malloc(sizeof(char) * (strlen((char*) var3) + 1));
							if (l_arr[index3].var == NULL)
								return INTERNAL_ERR;
						}
						memcpy(((char*) l_arr[index3].var), ((char*) var3), (strlen(((char*) var3)) + 1) * sizeof(char));
					}
					else
					{
						if (strlen(((char*) var3)) != strlen(((char*) g_arr[index3].var)))
						{
							free(g_arr[index3].var);
							g_arr[index3].var = NULL;
							g_arr[index3].var = malloc(sizeof(char) * (strlen((char*) var3) + 1));
							if (g_arr[index3].var == NULL)
								return INTERNAL_ERR;
						}
						memcpy(((char*) g_arr[index3].var), ((char*) var3), (strlen(((char*) var3)) + 1) * sizeof(char));
					}
					free(var1);
					free(var2);
					free(var3);
					var1 = NULL;
					var2 = NULL;
					var3 = NULL;
				}
				break;
			}

			case I_SUB: //-
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				if ((type1 != 2) && (type2 != 2) && (type3 != 2))
				{
					if (scope1 == 0)
						var1 = l_arr[index1].var;
					else
						var1 = g_arr[index1].var;

					if (scope2 == 0)
						var2 = l_arr[index2].var;
					else
						var2 = g_arr[index2].var;

					if (scope3 == 0)
						var3 = l_arr[index3].var;
					else
						var3 = g_arr[index3].var;

					if (type1 == 0)
					{
							if (type2 == 0)
								*((int*) var3) = *((int*) var1) - *((int*) var2);
							else
								*((double*) var3) = ((double)*((int*) var1)) - *((double*) var2);
					}
					else 
					{
						if (type1 == 1)
						{
							if (type2 == 0)
								*((double*) var3) = *((double*) var1) - ((double)*((int*) var2));
							else
								*((double*) var3) = *((double*) var1) - *((double*) var2);
						}
					}
				}
				break;
			}

			case I_MUL: // *
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 =  ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				if ((type1 != 2) && (type2 != 2) && (type3 != 2))
				{
					if (scope1 == 0)
						var1 = l_arr[index1].var;
					else
						var1 = g_arr[index1].var;

					if (scope2 == 0)
						var2 = l_arr[index2].var;
					else
						var2 = g_arr[index2].var;

					if (scope3 == 0)
						var3 = l_arr[index3].var;
					else
						var3 = g_arr[index3].var;

					if (type1 == 0)
					{
							if (type2 == 0)
								*((int*) var3) = (*((int*) var1)) * (*((int*) var2));
							else
								*((double*) var3) = ((double)*((int*) var1)) * (*((double*) var2));
					}
					else 
					{
						if (type1 == 1)
						{
							if (type2 == 0)
								*((double*) var3) = (*((double*) var1)) * ((double)*((int*) var2));
							else
								*((double*) var3) = (*((double*) var1)) * (*((double*) var2));
						}
					}
				}
				break;
			}

			case I_DIV: ///
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				if ((type1 != 2) && (type2 != 2) && (type3 != 2))
				{
					if (scope1 == 0)
						var1 = l_arr[index1].var;
					else
						var1 = g_arr[index1].var;

					if (scope2 == 0)
						var2 = l_arr[index2].var;
					else
						var2 = g_arr[index2].var;

					if (scope3 == 0)
						var3 = l_arr[index3].var;
					else
						var3 = g_arr[index3].var;

					if (type1 == 0)
					{
						if (type2 == 0)
						{
							if (*((int*) var2) == 0)
								return RUN_ZERO_ERROR;
							*((double*) var3) = ((double)*((int*) var1)) / ((double)(*((int*) var2)));
						}
						else
						{
							if (*((double*) var2) == 0.0)
								return RUN_ZERO_ERROR;
							*((double*) var3) = ((double)*((int*) var1)) / (*((double*) var2));
						}
					}
					else
					{
						if (type1 == 1)
						{
							if (type2 == 0)
							{
								if (*((int*) var2) == 0)
									return RUN_ZERO_ERROR;
								*((double*) var3) = (*((double*) var1)) / ((double)*((int*) var2));
							}
							else
							{
								if (*((double*) var2) == 0.0)
								{
									//clearall
									return RUN_ZERO_ERROR; //deleni nulou, behova chyba 8
								}
								*((double*) var3) = (*((double*) var1)) / (*((double*) var2));
							}
						}
					}
				}
				break;
			}

			//porovnavaci instrukce
			case I_GREAT: //>
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((int*) var1) > *((int*) var2);
						}
						break;
					}

					case 1:
					{
						if ((type2 == 1) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((double*) var1) > *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((bool*) var1) > *((bool*) var2);
						}
						break;
					}

					case 2:
					{
						if ((type2 == 2) && (type3 == 3))
						{
							if (scope1 == 0)
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) l_arr[index1].var), sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) g_arr[index1].var), sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) l_arr[index2].var), sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) g_arr[index2].var), sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							if (strcmp(var1,var2) > 0)
								*((bool*) var3) = true;
							else
								*((bool*) var3) = false;
							free(var1);
							free(var2);
						}
						break;
					}
				}
				break;
			}

			case I_SMALL: //<
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((int*) var1) < *((int*) var2);
						}
						break;
					}

					case 1:
					{
						if ((type2 == 1) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((double*) var1) < *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((bool*) var1) < *((bool*) var2);
						}
						break;
					}

					case 2:
					{
						if ((type2 == 2) && (type3 == 3))
						{
							if (scope1 == 0)
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) l_arr[index1].var), sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) g_arr[index1].var), sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) l_arr[index2].var), sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) g_arr[index2].var), sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							if (strcmp(var1,var2) < 0)
								*((bool*) var3) = true;
							else
								*((bool*) var3) = false;
							free(var1);
							free(var2);
						}
						break;
					}
				}
				break;
			}

			case I_EQUAL: //=
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;				

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((int*) var1) == *((int*) var2);
						}
						break;
					}

					case 1:
					{
						if ((type2 == 1) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((double*) var1) == *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((bool*) var1) == *((bool*) var2);
						}
						break;
					}

					case 2:
					{
						if ((type2 == 2) && (type3 == 3))
						{
							if (scope1 == 0)
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) l_arr[index1].var), sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) g_arr[index1].var), sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) l_arr[index2].var), sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) g_arr[index2].var), sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							if (strcmp(var1,var2) == 0)
								*((bool*) var3) = true;
							else
								*((bool*) var3) = false;
							free(var1);
							free(var2);
						}
						break;
					}
				}
				break;
			}

			case I_GREQ: //>=
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((int*) var1) >= *((int*) var2);
						}
						break;
					}

					case 1:
					{
						if ((type2 == 1) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((double*) var1) >= *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((bool*) var1) >= *((bool*) var2);
						}
						break;
					}

					case 2:
					{
						if ((type2 == 2) && (type3 == 3))
						{
							if (scope1 == 0)
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) l_arr[index1].var), sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) g_arr[index1].var), sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) l_arr[index2].var), sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) g_arr[index2].var), sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							if (strcmp(var1,var2) >= 0)
								*((bool*) var3) = true;
							else
								*((bool*) var3) = false;
							free(var1);
							free(var2);
						}
						break;
					}
				}
				break;
			}

			case I_SMEQ: //<=
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((int*) var1) <= *((int*) var2);
						}
						break;
					}

					case 1:
					{
						if ((type2 == 1) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((double*) var1) <= *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((bool*) var1) <= *((bool*) var2);
						}
						break;
					}

					case 2:
					{
						if ((type2 == 2) && (type3 == 3))
						{
							if (scope1 == 0)
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) l_arr[index1].var), sizeof(char) * (strlen(((char*) l_arr[index1].var)) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) g_arr[index1].var), sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen((char*) l_arr[index2].var) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) l_arr[index2].var), sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen((char*) g_arr[index2].var) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) g_arr[index2].var), sizeof(char) * (strlen((char*) g_arr[index2].var) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							if (strcmp(var1,var2) <= 0)
								*((bool*) var3) = true;
							else
								*((bool*) var3) = false;
							free(var1);
							free(var2);
						}
						break;
					}
				}
				break;
			}

			case I_NONEQ: //=!
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (scope1 == 0)
				{
					if (scope2 ==0)
					{
						if ((l_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((l_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				else
				{
					if (scope2 ==0)
					{
						if ((g_arr[index1].init == 0) || (l_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
					else
					{
						if ((g_arr[index1].init == 0) || (g_arr[index2].init == 0))
							return RUN_INIT_ERROR;
					}
				}
				
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((int*) var1) != *((int*) var2);
						}
						break;
					}

					case 1:
					{
						if ((type2 == 1) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((double*) var1) != *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1].var;
							else
								var1 = g_arr[index1].var;

							if (scope2 == 0)
								var2 = l_arr[index2].var;
							else
								var2 = g_arr[index2].var;

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							*((bool*) var3) = *((bool*) var1) != *((bool*) var2);
						}
						break;
					}

					case 2:
					{
						if ((type2 == 2) && (type3 == 3))
						{
							if (scope1 == 0)
							{
								var1 = malloc(sizeof(char) * (strlen((char*) l_arr[index1].var) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, l_arr[index1].var, sizeof(char) * (strlen((char*) l_arr[index1].var) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen((char*) g_arr[index1].var) + 1));
								if (var1 == NULL)
									return INTERNAL_ERR;
								memcpy(var1, ((char*) g_arr[index1].var), sizeof(char) * (strlen(((char*) g_arr[index1].var)) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen((char*) l_arr[index2].var) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) l_arr[index2].var), sizeof(char) * (strlen(((char*) l_arr[index2].var)) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen((char*) g_arr[index2].var) + 1));
								if (var2 == NULL)
									return INTERNAL_ERR;
								memcpy(var2, ((char*) g_arr[index2].var), sizeof(char) * (strlen(((char*) g_arr[index2].var)) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3].var;
							else
								var3 = g_arr[index3].var;

							if (strcmp(var1,var2) != 0)
								*((bool*) var3) = true;
							else
								*((bool*) var3) = false;
							free(var1);
							free(var2);
						}
						break;
					}
				}
				break;
			}

			//instrukce prirazeni

			/* I->addr1 - docasny ukazatel na prirazovanou hodnotu
			   I->addr3 - ukazatel na promennou v tabulce symbolu
			   Ziska si potrebne informace o promenne, vlozi docasny ukazatel na hodnotu do Garbage Collectoru,
			   nastavi inicializovanost promenne, do ktere se vklada hodnota, na 1 a vlozi do dane promenne hodnotu
			   z docasneho ukazatele. */
			case I_ASSIGN: //:=
			{
				if (I->addr1 == NULL)
					break;
				if ((garbage_add(Garbage, I->addr1)) == 1)
					return INTERNAL_ERR; 
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;
				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;
				
				switch (type3)
				{
					case 0:
					{
						if (scope3 == 0)
							*((int*) l_arr[index3].var) = *((int*) I->addr1);
						else
							*((int*) g_arr[index3].var) = *((int*) I->addr1);
						break;
					}

					case 1:
					{
						if (scope3 == 0)
							*((double*) l_arr[index3].var) = *((double*) I->addr1);
						else
							*((double*) g_arr[index3].var) = *((double*) I->addr1);
						break;
					}

					case 2:
					{
						if (scope3 == 0)
						{
							if (strlen((char*) I->addr1) != strlen((char*) l_arr[index3].var))
							{
								free(l_arr[index3].var);
								l_arr[index3].var = NULL;
								l_arr[index3].var = malloc(sizeof(char) * (strlen((char*) I->addr1) + 1));
								if (l_arr[index3].var == NULL)
									return INTERNAL_ERR;
							}
							memcpy(((char*) l_arr[index3].var), ((char*) I->addr1), (strlen((char*) I->addr1) + 1) * sizeof(char));
						}
						else
						{
							if (strlen((char*) I->addr1) != strlen((char*) g_arr[index3].var))
							{
								free(g_arr[index3].var);
								g_arr[index3].var = NULL;
								g_arr[index3].var = malloc(sizeof(char) * (strlen((char*) I->addr1) + 1));
								if (g_arr[index3].var == NULL)
									return INTERNAL_ERR;
							}
							memcpy(((char*) g_arr[index3].var), ((char*) I->addr1), (strlen((char*) I->addr1) + 1) * sizeof(char));
						}
						break;
					}

					case 3:
					{
						if (scope3 == 0)
							*((bool*) l_arr[index3].var) = *((bool*) I->addr1);
						else
							*((bool*) g_arr[index3].var) = *((bool*) I->addr1);
						break;
					}
				}
				break;
			}

			/* I->addr1 - ukazatel na promennou v tabulce symbolu, ta, co se kopiruje
			   I->addr3 - ukazatel na promennou v tabulce symbolu, ta, do ktere se kopiruje 
			   Ziska si potrebne informace o promennych, zkontroluje inicializovanost kopirovane promenne,
			   nastavi inicializovanost promenne, do ktere se kopiruje hodnota, na 1 a nakopiruje do dane promenne hodnotu
			   z kopirovane promenne. */
			case I_COPYVAR:
			{
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;
				
				if (scope1 == 0)
				{
					if (l_arr[index1].init == 0)
						return RUN_INIT_ERROR;
				}
				else
				{
					if (g_arr[index1].init == 0)
						return RUN_INIT_ERROR;
				}

				if (scope3 == 0)
					l_arr[index3].init = 1;
				else
					g_arr[index3].init = 1;

				switch (type1)
				{
					case 0:
					{
						if (scope3 == 0)
						{
							if (scope1 == 0)
								*((int*) l_arr[index3].var) = *((int*) l_arr[index1].var);
							else
								*((int*) l_arr[index3].var) = *((int*) g_arr[index1].var);
						}
						else
						{
							if (scope1 == 0)
								*((int*) g_arr[index3].var) = *((int*) l_arr[index1].var);
							else
								*((int*) g_arr[index3].var) = *((int*) g_arr[index1].var);
						}
						break;
					}

					case 1:
					{
						if (scope3 == 0)
						{
							if (scope1 == 0)
								*((double*) l_arr[index3].var) = *((double*) l_arr[index1].var);
							else
								*((double*) l_arr[index3].var) = *((double*) g_arr[index1].var);
						}
						else
						{
							if (scope1 == 0)
								*((double*) g_arr[index3].var) = *((double*) l_arr[index1].var);
							else
								*((double*) g_arr[index3].var) = *((double*) g_arr[index1].var);
						}
						break;
					}

					case 2:
					{
						if (scope3 == 0)
						{
							if (scope1 == 0)
							{
								if (strlen((char*) l_arr[index3].var) != strlen((char*) l_arr[index1].var))
								{
									free(l_arr[index3].var);
									l_arr[index3].var = NULL;
									l_arr[index3].var = malloc(sizeof(char) * (strlen((char*) l_arr[index1].var) + 1));
									if (l_arr[index3].var == NULL)
										return INTERNAL_ERR;
								}
								memcpy(((char*) l_arr[index3].var), ((char*) l_arr[index1].var), sizeof(char) * (strlen((char*) l_arr[index1].var) + 1));
							}
							else
							{
								if (strlen((char*) l_arr[index3].var) != strlen((char*) g_arr[index1].var))
								{
									free(l_arr[index3].var);
									l_arr[index3].var = NULL;
									l_arr[index3].var = malloc(sizeof(char) * (strlen((char*) g_arr[index1].var) + 1));
									if (l_arr[index3].var == NULL)
										return INTERNAL_ERR;
								}
								memcpy(((char*) l_arr[index3].var), ((char*) g_arr[index1].var), sizeof(char) * (strlen((char*) g_arr[index1].var) + 1));
							}
						}
						else
						{
							if (scope1 == 0)
							{
								if (strlen((char*) g_arr[index3].var) != strlen((char*) l_arr[index1].var))
								{
									free(g_arr[index3].var);
									g_arr[index3].var = NULL;
									g_arr[index3].var = malloc(sizeof(char) * (strlen((char*) l_arr[index1].var) + 1));
									if (g_arr[index3].var == NULL)
										return INTERNAL_ERR;
								}
								memcpy(((char*) g_arr[index3].var), ((char*) l_arr[index1].var), sizeof(char) * (strlen((char*) l_arr[index1].var) + 1));
							}
							else
							{
								if (strlen((char*) g_arr[index3].var) != strlen((char*) g_arr[index1].var))
								{
									free(g_arr[index3].var);
									g_arr[index3].var = NULL;
									g_arr[index3].var = malloc(sizeof(char) * (strlen((char*) g_arr[index1].var) + 1));
									if (g_arr[index3].var == NULL)
										return INTERNAL_ERR;
								}
								memcpy(((char*) g_arr[index3].var), ((char*) g_arr[index1].var), sizeof(char) * (strlen((char*) g_arr[index1].var) + 1));
							}
						}
						break;
					}

					case 3:
					{
						if (scope3 == 0)
						{
							if (scope1 == 0)
								*((bool*) l_arr[index3].var) = *((bool*) l_arr[index1].var);
							else
								*((bool*) l_arr[index3].var) = *((bool*) g_arr[index1].var);
						}
						else
						{
							if (scope1 == 0)
								*((bool*) g_arr[index3].var) = *((bool*) l_arr[index1].var);
							else
								*((bool*) g_arr[index3].var) = *((bool*) g_arr[index1].var);
						}
						break;
					}
				}

				break;
			}


			//instrukce pro volani funkci

			/* I->addr3 - ukazatel na promennou v tabulce symbolu
			   Nacte si potrebne informace o promenne, ulozi si jeji hodnotu inicializace do docasneho ukazatele,
			   vlozi na zasobnik hodnotu promenne a nasledne vlozi na zasobnik jeji hodnotu inicializace. */
			case I_PUSH_PARAM:
			{
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;
				if (scope3 == 0)
					*init_temp = l_arr[index3].init;
				else
					*init_temp = g_arr[index3].init;
				switch (type3)
				{
					case 0:
					{
						if (scope3 == 0)
						{
							if ((VarStackPush(VS, ((int*) l_arr[index3].var), NULL, NULL, NULL)) == INTERNAL_ERR)
								return INTERNAL_ERR;
						}
						else
						{
							if ((VarStackPush(VS, ((int*) g_arr[index3].var), NULL, NULL, NULL)) == INTERNAL_ERR)
								return INTERNAL_ERR;
						}
						break;
					}

					case 1:
					{
						if (scope3 == 0)
						{
							if ((VarStackPush(VS, NULL, ((double*) l_arr[index3].var), NULL, NULL)) == INTERNAL_ERR)
								return INTERNAL_ERR;
						}
						else
						{
							if ((VarStackPush(VS, NULL, ((double*) g_arr[index3].var), NULL, NULL)) == INTERNAL_ERR)
								return INTERNAL_ERR;
						}
						break;
					}

					case 2:
					{
						if (scope3 == 0)
						{
							if ((VarStackPush(VS, NULL, NULL, ((char*) l_arr[index3].var), NULL)) == INTERNAL_ERR)
								return INTERNAL_ERR;
						}
						else
						{
							if ((VarStackPush(VS, NULL, NULL, ((char*) g_arr[index3].var), NULL)) == INTERNAL_ERR)
								return INTERNAL_ERR;
						}
						break;
					}

					case 3:
					{
						if (scope3 == 0)
						{
							if ((VarStackPush(VS, NULL, NULL, NULL, ((bool*) l_arr[index3].var))) == INTERNAL_ERR)
								return INTERNAL_ERR;
						}
						else
						{
							if ((VarStackPush(VS, NULL, NULL, NULL, ((bool*) g_arr[index3].var))) == INTERNAL_ERR)
								return INTERNAL_ERR;
						}
						break;
					}
				}

				if ((VarStackPush(VS, NULL, NULL, NULL, init_temp)) == INTERNAL_ERR)
					return INTERNAL_ERR;
				break;
			}

			/* I->addr1 - adresa volane funkce v tabulce symbolu
			   I->addr3 - adresa promenne v tabulce symbolu
			   Ziska si potrebne informace o promenne, do ktere se ulozi navratova hodnota.
			   Nasledne, pokud existuje, vlozi na zasobnik lokalnich ramcu momentalni ramec.
			   Pote si nacte novy lokalni ramec, odpovidajici tabulce symbolu volane funkce.
			   V dalsich krocich postupne zkontroluje inicializovanost parametru a nacte jejich hodnoty ze zasobniku.
			   Dale vlozi na zasobnik navratovych adres momentalni navratovou adresu a nacte si novou.
			   Nakonec vlozi na zasobnik promennych informace o promenne, do ktere se ulozi navratova hodnota a posune
			   se na adresu zacatku volane funkce. */
			case I_CALL:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*type_temp = ((htab_item*) I->addr3)->type;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;

				if (l_arr != NULL)
					if ((LStackPush(LS, l_arr)) == INTERNAL_ERR)
						return INTERNAL_ERR;

				l_arr = NULL;
				l_arr = malloc(sizeof(tVarArr) * (((htab_item*) I->addr1)->func_table->item_count));
				if (l_arr == NULL)
					return INTERNAL_ERR;
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				if ((loadarray(l_arr, ((htab_item*) I->addr1)->func_table)) == INTERNAL_ERR)
					return INTERNAL_ERR;

				if ((((htab_item*) I->addr1)->func_data) != NULL)
					i = strlen(((htab_item*) I->addr1)->func_data);
				else
					i = 0;

				while (i > 0)
				{
					char *temp;
					l_arr[i].init = BoolVarStackPop(VS);
					switch (((htab_item*) I->addr1)->func_data[(i-1)]) //cteni znaku retezce typu - strlen ==3 -> posledni znak je na pozici [2]
					{
						case 'i':
						{
							*((int*) l_arr[i].var) = IntVarStackPop(VS);
							break;
						}

						case 's':
						{
							temp = StrVarStackPop(VS);
							if (strlen(temp) != strlen((char*) l_arr[i].var))
							{
								free(l_arr[i].var);
								l_arr[i].var = NULL;
								l_arr[i].var = malloc(sizeof(char) * (strlen(temp) + 1));
								if (l_arr[i].var == NULL)
									return INTERNAL_ERR;
							}
							memcpy(((char*) l_arr[i].var), temp, sizeof(char) * (strlen(temp) + 1));
							break;	
						}

						case 'r':
						{
							*((double*) l_arr[i].var) = DoubleVarStackPop(VS);
							break;
						}

						case 'b':
						{
							*((bool*) l_arr[i].var) = BoolVarStackPop(VS);
							break;
						}
					}
					i--;
				}
				
				
				if ((VarStackPush(VS, index_temp, NULL, NULL, NULL)) == INTERNAL_ERR)
					return INTERNAL_ERR;
				if ((VarStackPush(VS, NULL, NULL, NULL, scope_temp)) == INTERNAL_ERR)
					return INTERNAL_ERR;
				if ((VarStackPush(VS, size_temp, NULL, NULL, NULL)) == INTERNAL_ERR)
					return INTERNAL_ERR;
				if ((VarStackPush(VS, type_temp, NULL, NULL, NULL)) == INTERNAL_ERR)
					return INTERNAL_ERR;
				if (return_addr != NULL)
					if ((AddStackPush(AS, return_addr)) == INTERNAL_ERR)
						return INTERNAL_ERR;
				return_addr = CurrentItemAddress(List);
				
				GoToItem(List, ((htab_item*) I->addr1)->label);
				break;
			}

			/* Nacte si ze zasobniku informace o promenne, do ktere se ulozi navratova hodnota momentalne spracovavane funkce.
			   Pokud byla navratova hodnota inicializovana, ulozi ji do docasne promenne, uvolni momentalni lokalni ramec hodnot a, pokud existuje,
			   nacte nadrazeny lokalni ramec, ulozi navratovou hodnotu do zadane promenne, posune se v seznamu isntrukci na
			   navratovou hodnotu a nacte ze zasobniku novou navratovou hodnotu. */
			case I_RETURN:
			{
				*type_temp = IntVarStackPop(VS);
				*size_temp = IntVarStackPop(VS);
				*scope_temp = BoolVarStackPop(VS);
				*index_temp = IntVarStackPop(VS);
				
				if (l_arr[0].init == 0)
					return RUN_INIT_ERROR;
				switch (*type_temp)
				{
					case 0:
					{
						value_temp = malloc(sizeof(int));
						if (value_temp == NULL)
							return INTERNAL_ERR;
						*((int*) value_temp) = *((int*) l_arr[0].var);
						break;
					}

					case 1:
					{
						value_temp = malloc(sizeof(double));
						if (value_temp == NULL)
							return INTERNAL_ERR;
						*((double*) value_temp) = *((double*) l_arr[0].var);
						break;
					}

					case 2:
					{
						value_temp = malloc(sizeof(char) * (strlen((char*) l_arr[0].var) + 1));
						if (value_temp == NULL)
							return INTERNAL_ERR;
						memcpy(((char*) value_temp), ((char*) l_arr[0].var), sizeof(char) * (strlen((char*) l_arr[0].var) + 1));
						break;
					}

					case 3:
					{
						value_temp = malloc(sizeof(bool));
						if (value_temp == NULL)
							return INTERNAL_ERR;
						*((bool*) value_temp) = *((bool*) l_arr[0].var);
						break;
					}
				}

				disposearray(l_arr, (*size_temp));
				l_arr = NULL;
				if (!(LStackEmpty(LS)))
					l_arr = LStackPop(LS);

				if (*scope_temp == 0)
				{
					switch (*type_temp)
					{
						case 0:
						{
							*((int*) l_arr[*index_temp].var) = *((int*) value_temp);
							break;
						}

						case 1:
						{
							*((double*) l_arr[*index_temp].var) = *((double*) value_temp);
							break;
						}

						case 2:
						{
							if ((strlen((char*) value_temp)) != strlen((char*) l_arr[*index_temp].var))
							{
								free(l_arr[*index_temp].var);
								l_arr[*index_temp].var = NULL;
								l_arr[*index_temp].var = malloc(sizeof(char) * (strlen((char*) value_temp) + 1));
								if (l_arr[*index_temp].var == NULL)
									return INTERNAL_ERR;
							}
							memcpy(((char*) l_arr[*index_temp].var), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
							break;
						}

						case 3:
						{
							*((bool*) l_arr[*index_temp].var) = *((bool*) value_temp);
							break;
						}
					}

				}
				else
				{
					switch (*type_temp)
					{
						case 0:
						{
							*((int*) g_arr[*index_temp].var) = *((int*) value_temp);
							break;
						}

						case 1:
						{
							*((double*) g_arr[*index_temp].var) = *((double*) value_temp);
							break;
						}

						case 2:
						{
							if ((strlen((char*) value_temp)) != strlen((char*) g_arr[*index_temp].var))
							{
								free(g_arr[*index_temp].var);
								g_arr[*index_temp].var = NULL;
								g_arr[*index_temp].var = malloc(sizeof(char) * (strlen((char*) value_temp) + 1));
								if (g_arr[*index_temp].var == NULL)
									return INTERNAL_ERR;
							}
							memcpy(((char*) g_arr[*index_temp].var), ((char*) value_temp), (sizeof(char) * (strlen((char*) value_temp) + 1)));
							break;
						}

						case 3:
						{
							*((bool*) g_arr[*index_temp].var) = *((bool*) value_temp);
							break;
						}
					}
				}

				if (*scope_temp == 0)
					l_arr[*index_temp].init = 1;
				else
					g_arr[*index_temp].init = 1;

				free(value_temp);
				GoToItem(List, return_addr);
				return_addr = AddStackPop(AS);				
				break;
			}
		}
		NextIns(List);
	}
}
