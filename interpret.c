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

void initarray(tVarArr *array, int size)
{
	for (int i = 0; i < size; i++)
		array[i].var = NULL;
}

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

	tAddS *AS = malloc(sizeof(tAddS));
	if (AS == NULL)
		return INTERNAL_ERR;
	AS->add_stack = malloc(sizeof(tItem)*(ADS_STACK_SIZE));
	if (AS->add_stack == NULL)
		return INTERNAL_ERR;
	AddStackInit(AS, ADS_STACK_SIZE);

//pri ruseni velikost u funkce v tabulce
	
//load global array
//initiate stacks
	while (1)
	{
		I = GetData(List);
		switch (I->Type)
		{
			//pomocne instrukce
			case I_LABEL:
				break;

			case I_IFGOTO:
			{
			//get info about addr1
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				if (type1 != 2)
				{
					if (scope1 == 0)
					{
						if (!(*((bool*) l_arr[index1].var)))
							GoToItem(List,((tItem*) I->addr3)); //melo by fungovat...
					}
					else
					{
						if (!(*((bool*) g_arr[index1].var)))
							GoToItem(List,((tItem*) I->addr3));
					}
				}
				break;
			}

			case I_GOTO:
			{
				GoToItem(List,((tItem*) I->addr3));
				break;
			}

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
			/*case I_READ:
			{
				break;
			}*/

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
					{
						//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
						return RUN_INIT_ERROR; //prace s neinicializovanou promennou, behova chyba 7
					}
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

			//nase instrukce
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
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					return RUN_INIT_ERROR; //prace s neinicializovanou promennou, behova chyba 7
				}

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
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					return RUN_INIT_ERROR; //prace s neinicializovanou promennou, behova chyba 7
				}

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

			case I_SORT:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;
				((htab_item*) I->addr3)->initialized = 1;

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
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					return RUN_INIT_ERROR; //prace s neinicializovanou promennou, behova chyba 7
				}

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
					l_arr[*index_temp].init = 1;
				}

				free(value_temp);
				free(SPtr);
				SPtr = NULL;
				break;
			}

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
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					return RUN_INIT_ERROR; //prace s neinicializovanou promennou, behova chyba 7
				}

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

			case I_COPY:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;
				((htab_item*) I->addr3)->initialized = 1;

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
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					return RUN_INIT_ERROR; //prace s neinicializovanou promennou, behova chyba 7
				}

				*((int*) l_arr[3].var) = IntVarStackPop(VS);

				if ((BoolVarStackPop(VS)) == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					return RUN_INIT_ERROR; //prace s neinicializovanou promennou, behova chyba 7
				}

				*((int*) l_arr[2].var) = IntVarStackPop(VS);

				if ((BoolVarStackPop(VS)) == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					return RUN_INIT_ERROR; //prace s neinicializovanou promennou, behova chyba 7
				}

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

				if (*((int*) value_temp) == SEM_ERROR)
					return SEM_ERROR;

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
			case I_ADD: //+
			{
				//get type, scope, index

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
					//error - asi neni potreba kontrolovat
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
							{
								//clearall - nejdriv uvolnit vsechny local array, pak global a vse 												 ostatni
								return RUN_ZERO_ERROR; //deleni nulou, behova chyba 8
							}
							*((double*) var3) = ((double)*((int*) var1)) / ((double)(*((int*) var2)));
						}
						else
						{
							if (*((double*) var2) == 0.0)
							{
								//clearall
								return RUN_ZERO_ERROR; //deleni nulou, behova chyba 8
							}
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
								{
									//clearall
									return RUN_ZERO_ERROR; //deleni nulou, behova chyba 8
								}
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

			/*case I_INC: //++
			{
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (type3 != 2)
				{
					if (scope3 == 0)
						var3 = l_arr[index3].var;
					else
						var3 = g_arr[index3].var;

					if (type3 == 0)
						(*((int*) var3))++;
					else
						(*((double*) var3))++;
				}
				break;
			}

			case I_DEC: //--
			{
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (type3 != 2)
				{
					if (scope3 == 0)
						var3 = l_arr[index3].var;
					else
						var3 = g_arr[index3].var;

					if (type3 == 0)
						(*((int*) var3))--;
					else
						(*((double*) var3))--;
				}
				break;
			}*/

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
						//error?
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
						//error?
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
						//error?
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
						//error?
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
						//error?
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
						//error?
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
				//addr3 - address of storage variable
				//addr1 - temp pointer for value
				//---------------------------------
				//search for index, type and scope
				//copy value into array
				//free received pointer
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
			case I_PUSH_PARAM:
			{
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;
				if (scope3 == 0)
					*init_temp = l_arr[index3].init;
				else
					*init_temp = g_arr[index3].init;
				//search table for type, index and scope
				//push value from array
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

			case I_CALL:
			{
				//addr1 - adresa volane funkce - get label, get number of variables
				//addr3 - adresa promenne pro ulozeni return value
				//search for index and type of return value
				//push local array
				//load new local array
				//get parameter count
				//pop & load paramaters
				//push index of return value in calling function
				//push current return address
				//get new return address - adress current instruction (after return, NextIns(List) is used
				// addr1

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

			case I_RETURN:
			{
				//pop return value index, type, scope
				//save return value (index 0) into temporary variable
				//free local table
				//pop now-current local table -- if stack isn't empty
				//load return value into index (realloc string if needed)
				//goto return address
				//pop now-current return address

				*type_temp = IntVarStackPop(VS);
				*size_temp = IntVarStackPop(VS);
				*scope_temp = BoolVarStackPop(VS);
				*index_temp = IntVarStackPop(VS);
				
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
