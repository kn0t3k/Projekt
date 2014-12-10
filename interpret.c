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


#define VAR_STACK_SIZE 50
#define ADS_STACK_SIZE 50
#define LVS_STACK_SIZE 50

void initarray(void **array, int size)
{
	for (int i = 0; i < size; i++)
		array[i] = NULL;
}

void loadarray(void **array, symbol_table_item *TB)
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
				iptr->initialized = 0;
				switch (type)
				{
					case 0:
					{
						array[iptr->index] = malloc(sizeof(int));
						break;
					}

					case 1:
					{
						array[iptr->index] = malloc(sizeof(double));
						break;
					}

					case 2:
					{
						array[iptr->index] = malloc(sizeof(char));
						((char*) array[iptr->index])[0] = '\0';
						break;
					}

					case 3:
					{
						array[iptr->index] = malloc(sizeof(bool));
						break;
					}

					default:
						break;
				}
			}
			iptr = iptr->next;
		}
	}
} 

void disposearray(void **array, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (array[i] != NULL)
			free(array[i]);
	}

	if (array != NULL)
		free(array);
}

htab_item* FindItemOfIndex(int index, symbol_table_item *TB)
{
	int hash_size = TB->table->htab_size;
	htab_item *iptr = NULL;

	for (int i = 0; i < hash_size; i++)
	{
		iptr = TB->table->ptr[i];
		while (iptr != NULL)
		{
			if (iptr->index == index)
				return iptr;			
			iptr = iptr->next;
		}
	}
	return NULL;
}

int interpret(symbol_table_item *GTable, tList *List)
{
	First(List);
	if (List == NULL)
		return 0;
	tInstr *I;
	string *SPtr1;
	string *SPtr2;
	tPrintList *PrintList = malloc(sizeof(tPrintList));
	InitPrintList(PrintList);
	char *str_temp;
	int *index_temp = malloc(sizeof(int));
	int *size_temp = malloc(sizeof(int));
	int *type_temp = malloc(sizeof(int));	
	int index1 = -1;
	int index2 = -1;
	int index3 = -1;
	int type1 = -1;
	int type2 = -1;
	int type3 = -1;
	int i = 0;
	bool *init_temp = malloc(sizeof(bool));
	bool *scope_temp = malloc(sizeof(bool));
	bool scope1 = -1; //pro scope plati: 0 == local, 1 == global
	bool scope2 = -1;
	bool scope3 = -1;
	void *value_temp;
	void *return_addr = NULL;
	void *var1 = NULL;
	void *var2 = NULL;
	void *var3 = NULL;
	tVarS *VS = malloc(sizeof(tVarS)); //variable stack
	VS->var_stack = malloc(sizeof(void*) * (VAR_STACK_SIZE));
	VarStackInit(VS,VAR_STACK_SIZE);
	void **g_arr = malloc(sizeof(void*) * GTable->item_count);
	initarray(g_arr, GTable->item_count);
	loadarray(g_arr, GTable);
	void **l_arr = NULL;

	tLVS *LS = malloc(sizeof(tLVS)); //local array stack
	LS->l_stack = malloc(sizeof(vararr) * (LVS_STACK_SIZE));
	LStackInit(LS, LVS_STACK_SIZE);

	tAddS *AS = malloc(sizeof(tAddS));
	AS->add_stack = malloc(sizeof(tItem)*(ADS_STACK_SIZE));
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
						if (!(*((bool*) l_arr[index1])))
							GoToItem(List,((tItem*) I->addr3)); //melo by fungovat...
					}
					else
					{
						if (!(*((bool*) g_arr[index1])))
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
				DisposeVarStack(VS);
				disposearray(g_arr, GTable->item_count);
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

				for (int i = (*size_temp - 1); i >= 0; i--)
				{
					//char *temp;
					if ((BoolVarStackPop(VS)) == 0)
					{
						//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
						printf("Neinicializovany operand."); //smazat!!
						return 7; //prace s neinicializovanou promennou, behova chyba 7
					}
					switch (((char*) I->addr3)[i])
					{
						case 'i':
						{
							value_temp = malloc(sizeof(int));
							*((int*) value_temp) = IntVarStackPop(VS);
							//printf("%d", *((int*) value_temp));
							//free(value_temp);
							//value_temp = NULL;
							InsertPrintNew(PrintList, 0, ((void*) value_temp));
							break;
						}

						case 'r':
						{
							value_temp = malloc(sizeof(double));
							*((double*) value_temp) = DoubleVarStackPop(VS);
							//printf("%g", *((double*) value_temp));
							//free(value_temp);
							//value_temp = NULL;
							InsertPrintNew(PrintList, 1, ((void*) value_temp));
							break;
						}
			
						case 's':
						{
							InsertPrintNew(PrintList, 2, ((void*) StrVarStackPop(VS)));
							break;
						}

						case 'b':
						{
							value_temp = malloc(sizeof(bool));
							*((bool*) value_temp) = BoolVarStackPop(VS);
							//printf("%d", *((bool*) value_temp));
							//free(value_temp);
							//value_temp = NULL;
							InsertPrintNew(PrintList, 3, ((void*) value_temp));
							break;
						}
					}
							
				}
				PrintAll(PrintList);
				//free(PrintList);
				//PrintList = malloc(sizeof(tPrintList));
				//InitPrintList(PrintList);
				/*if (I->addr3 != NULL)
					free(I->addr3);*/
				break;
			}

			//nase instrukce
			case I_FIND:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;
				((htab_item*) I->addr3)->initialized = 1;

				if (l_arr != NULL)
					LStackPush(LS, l_arr);

				l_arr = NULL;
				l_arr = malloc(sizeof(void*) * (((htab_item*) I->addr1)->func_table->item_count));
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				loadarray(l_arr, ((htab_item*) I->addr1)->func_table);

				if ((BoolVarStackPop(VS)) == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}

				str_temp = StrVarStackPop(VS);
				if (strlen(str_temp) != strlen((char*) l_arr[2]))
				{
					free(l_arr[2]);
					l_arr[2] = NULL;
					l_arr[2] = malloc(sizeof(char) * (strlen(str_temp) + 1));
				}
				memcpy(((char*) l_arr[2]), str_temp, sizeof(char) * (strlen(str_temp) + 1));
				//free(str_temp); ??

				if ((BoolVarStackPop(VS)) == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}

				str_temp = StrVarStackPop(VS);
				if (strlen(str_temp) != strlen((char*) l_arr[1]))
				{
					free(l_arr[1]);
					l_arr[1] = NULL;
					l_arr[1] = malloc(sizeof(char) * (strlen(str_temp) + 1));
				}
				memcpy(((char*) l_arr[1]), str_temp, sizeof(char) * (strlen(str_temp) + 1));
				//free(str_temp); ??

				value_temp = malloc(sizeof(int));
				*((int*) value_temp) = find(((char*) l_arr[1]), ((char*) l_arr[2]), strlen((char*) l_arr[1]), strlen((char*) l_arr[2]));

				disposearray(l_arr, (*size_temp));
				l_arr = NULL;
				if (!(LStackEmpty(LS)))
					l_arr = LStackPop(LS);

				if (*scope_temp == 0)
					*((int*) l_arr[*index_temp]) = *((int*) value_temp);
				else
					*((int*) g_arr[*index_temp]) = *((int*) value_temp);

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
					LStackPush(LS, l_arr);

				l_arr = NULL;
				l_arr = malloc(sizeof(void*) * (((htab_item*) I->addr1)->func_table->item_count));
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				loadarray(l_arr, ((htab_item*) I->addr1)->func_table);

				if ((BoolVarStackPop(VS)) == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}

				str_temp = StrVarStackPop(VS);
				
				if (strlen(str_temp) != strlen((char*) l_arr[0]))
				{
					free(l_arr[0]);
					l_arr[0] = NULL;
					l_arr[0] = malloc(sizeof(char) * (strlen(str_temp) + 1));
				}
				memcpy(((char*) l_arr[0]), str_temp, sizeof(char) * (strlen(str_temp) + 1));
				//free(str_temp) ??
				SPtr1 = malloc(sizeof(string));
				SPtr1->str = ((char*) l_arr[0]);
				SPtr1->length = strlen((char*) l_arr[0]);
				SPtr1->allocSize = SPtr1->length + 1;
				
				value_temp = malloc(sizeof(int));
					*((int*) value_temp) = sort(SPtr1);
				
				if ((*((int*) value_temp)) == 1)
					printf("chyba\n");

				free(value_temp);
				value_temp = NULL;
				value_temp = malloc(sizeof(char) * (strlen((char*) l_arr[0]) + 1));
				memcpy(((char*) value_temp), ((char*) l_arr[0]), sizeof(char) * (strlen((char*) l_arr[0]) + 1));

				disposearray(l_arr, (*size_temp));
				l_arr = NULL;
				if (!(LStackEmpty(LS)))
					l_arr = LStackPop(LS);

				if (*scope_temp == 0)
				{
					if (strlen((char*) l_arr[*index_temp]) != strlen((char*) value_temp))
					{
						free(l_arr[*index_temp]);
						l_arr[*index_temp] = NULL;
						l_arr[*index_temp] = malloc(sizeof(char) * (strlen(value_temp) + 1));
					}
					memcpy(((char*) l_arr[*index_temp]), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
				}
				else
				{
					if (strlen((char*) g_arr[*index_temp]) != strlen((char*) value_temp))
					{
						free(g_arr[*index_temp]);
						g_arr[*index_temp] = NULL;
						g_arr[*index_temp] = malloc(sizeof(char) * (strlen(value_temp) + 1));
					}
					memcpy(((char*) g_arr[*index_temp]), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
				}

				free(value_temp);
				free(SPtr1);
				SPtr1 = NULL;
				free(SPtr2);
				SPtr2 = NULL;
				break;
			}

			case I_LENGTH:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;
				((htab_item*) I->addr3)->initialized = 1;

				if (l_arr != NULL)
					LStackPush(LS, l_arr);

				l_arr = NULL;
				l_arr = malloc(sizeof(void*) * (((htab_item*) I->addr1)->func_table->item_count));
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				loadarray(l_arr, ((htab_item*) I->addr1)->func_table);

				if ((BoolVarStackPop(VS)) == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}

				str_temp = StrVarStackPop(VS);
				
				if (strlen(str_temp) != strlen((char*) l_arr[1]))
				{
					free(l_arr[1]);
					l_arr[1] = NULL;
					l_arr[1] = malloc(sizeof(char) * (strlen(str_temp) + 1));
				}
				memcpy(((char*) l_arr[1]), str_temp, sizeof(char) * (strlen(str_temp) + 1));
				//free(str_temp); ??

				value_temp = malloc(sizeof(int));

				SPtr1 = malloc(sizeof(string));
				SPtr1->str = ((char*) l_arr[1]);
				SPtr1->length = strlen((char*) l_arr[1]);
				SPtr1->allocSize = SPtr1->length + 1;

				*((int*) value_temp) = length(SPtr1);

				disposearray(l_arr, (*size_temp));
				l_arr = NULL;
				if (!(LStackEmpty(LS)))
					l_arr = LStackPop(LS);

				if (*scope_temp == 0)
					*((int*) l_arr[*index_temp]) = *((int*) value_temp);
				else
					*((int*) g_arr[*index_temp]) = *((int*) value_temp);

				free(value_temp);
				free(SPtr1);
				SPtr1 = NULL;
				break;
			}

			case I_COPY:
			{
				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;
				((htab_item*) I->addr3)->initialized = 1;

				if (l_arr != NULL)
					LStackPush(LS, l_arr);

				l_arr = NULL;
				l_arr = malloc(sizeof(void*) * (((htab_item*) I->addr1)->func_table->item_count));
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				loadarray(l_arr, ((htab_item*) I->addr1)->func_table);

				if ((BoolVarStackPop(VS)) == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}

				*((int*) l_arr[3]) = IntVarStackPop(VS);

				if ((BoolVarStackPop(VS)) == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}

				*((int*) l_arr[2]) = IntVarStackPop(VS);

				if ((BoolVarStackPop(VS)) == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}

				str_temp = StrVarStackPop(VS);
				if (strlen(str_temp) != strlen((char*) l_arr[1]))
				{
					free(l_arr[1]);
					l_arr[1] = NULL;
					l_arr[1] = malloc(sizeof(char) * (strlen(str_temp) + 1));
				}
				memcpy(((char*) l_arr[1]), str_temp, sizeof(char) * (strlen(str_temp) + 1));

				if (strlen(str_temp) != strlen((char*) l_arr[0]))
				{
					free(l_arr[0]);
					l_arr[0] = NULL;
					l_arr[0] = malloc(sizeof(char) * (strlen(str_temp) + 1));
				}
				memcpy(((char*) l_arr[0]), str_temp, sizeof(char) * (strlen(str_temp) + 1));

				SPtr1 = malloc(sizeof(string));
				SPtr1->str = ((char*) l_arr[0]);
				SPtr1->length = strlen((char*) l_arr[0]);
				SPtr1->allocSize = SPtr1->length + 1;
				value_temp = malloc(sizeof(int));

				*((int*) value_temp) = copy(((char*) l_arr[1]), SPtr1, *((int*) l_arr[2]), *((int*) l_arr[3]), strlen((char*) l_arr[1]));

				if (*((int*) value_temp) == SEM_ERROR)
					return SEM_ERROR;

				free(value_temp);
				value_temp = NULL;
				value_temp = malloc(sizeof(char) * (strlen((char*) l_arr[0]) + 1));

				memcpy(((char*) value_temp), ((char*) l_arr[0]), sizeof(char) * (strlen((char*) l_arr[0]) + 1));

				
				disposearray(l_arr, (*size_temp));
				l_arr = NULL;
				if (!(LStackEmpty(LS)))
					l_arr = LStackPop(LS);

				if (*scope_temp == 0)
				{
					if (strlen((char*) l_arr[*index_temp]) != strlen((char*) value_temp))
					{
						free(l_arr[*index_temp]);
						l_arr[*index_temp] = NULL;
						l_arr[*index_temp] = malloc(sizeof(char) * (strlen(value_temp) + 1));
					}
					memcpy(((char*) l_arr[*index_temp]), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
				}
				else
				{
					if (strlen((char*) g_arr[*index_temp]) != strlen((char*) value_temp))
					{
						free(g_arr[*index_temp]);
						g_arr[*index_temp] = NULL;
						g_arr[*index_temp] = malloc(sizeof(char) * (strlen(value_temp) + 1));
					}
					memcpy(((char*) g_arr[*index_temp]), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
				}

				free(value_temp);
				free(SPtr1);
				SPtr1 = NULL;
				break;
			}

			//aritmeticke instrukce
			case I_ADD: //+
			{
				//get type, scope, index

				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if ((type1 != 2) && (type2 != 2) && (type3 != 2))
				{
					if (scope1 == 0)
						var1 = l_arr[index1];
					else
						var1 = g_arr[index1];

					if (scope2 == 0)
						var2 = l_arr[index2];
					else
						var2 = g_arr[index2];

					if (scope3 == 0)
						var3 = l_arr[index3];
					else
						var3 = g_arr[index3];

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
						var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
						memcpy(((char*) var1), ((char*) l_arr[index1]), strlen(((char*) l_arr[index1])) + 1);
					}
					else
					{
						var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
						memcpy(((char*) var1), ((char*) g_arr[index1]), strlen(((char*) g_arr[index1])) + 1);
					}

					if (scope2 == 0)
					{
						var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
						memcpy(((char*) var2), ((char*) l_arr[index2]), strlen(((char*) l_arr[index2])) + 1);
					}
					else
					{
						var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
						memcpy(((char*) var2), ((char*) g_arr[index2]), strlen(((char*) g_arr[index2])) + 1);
					}

					var3 = malloc(sizeof(char) * (strlen(var1) + strlen(var2) + 1));
					memcpy(((char*) var3), ((char*) var1), (strlen((char*) var1) + 1) * sizeof(char));
					strncat(((char*) var3), ((char*) var2), strlen(((char*) var2)));

					if (scope3 == 0)
					{
						if (strlen(((char*) var3)) != strlen(((char*) l_arr[index3])))
						{
							free(l_arr[index3]);
							l_arr[index3] = NULL;
							l_arr[index3] = malloc(sizeof(char) * (strlen((char*) var3) + 1));
						}
						memcpy(((char*) l_arr[index3]), ((char*) var3), (strlen(((char*) var3)) + 1) * sizeof(char));
					}
					else
					{
						if (strlen(((char*) var3)) != strlen(((char*) g_arr[index3])))
						{
							free(g_arr[index3]);
							g_arr[index3] = NULL;
							g_arr[index3] = malloc(sizeof(char) * (strlen((char*) var3) + 1));
						}
						memcpy(((char*) g_arr[index3]), ((char*) var3), (strlen(((char*) var3)) + 1) * sizeof(char));
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
				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if ((type1 != 2) && (type2 != 2) && (type3 != 2))
				{
					if (scope1 == 0)
						var1 = l_arr[index1];
					else
						var1 = g_arr[index1];

					if (scope2 == 0)
						var2 = l_arr[index2];
					else
						var2 = g_arr[index2];

					if (scope3 == 0)
						var3 = l_arr[index3];
					else
						var3 = g_arr[index3];

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
				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 =  ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if ((type1 != 2) && (type2 != 2) && (type3 != 2))
				{
					if (scope1 == 0)
						var1 = l_arr[index1];
					else
						var1 = g_arr[index1];

					if (scope2 == 0)
						var2 = l_arr[index2];
					else
						var2 = g_arr[index2];

					if (scope3 == 0)
						var3 = l_arr[index3];
					else
						var3 = g_arr[index3];

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
				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if ((type1 != 2) && (type2 != 2) && (type3 != 2))
				{
					if (scope1 == 0)
						var1 = l_arr[index1];
					else
						var1 = g_arr[index1];

					if (scope2 == 0)
						var2 = l_arr[index2];
					else
						var2 = g_arr[index2];

					if (scope3 == 0)
						var3 = l_arr[index3];
					else
						var3 = g_arr[index3];

					if (type1 == 0)
					{
						if (type2 == 0)
						{
							if (*((int*) var2) == 0)
							{
								//clearall - nejdriv uvolnit vsechny local array, pak global a vse 												 ostatni
								printf("Behova chyba, deleni nulou."); //smazat!!
								return 8; //deleni nulou, behova chyba 8
							}
							*((double*) var3) = ((double)*((int*) var1)) / ((double)(*((int*) var2)));
						}
						else
						{
							if (*((double*) var2) == 0.0)
							{
								//clearall
								printf("Behova chyba, deleni nulou."); //smazat!!
								return 8; //deleni nulou, behova chyba 8
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
									printf("Behova chyba, deleni nulou."); //smazat!!
									return 8; //deleni nulou, behova chyba 8
								}
								*((double*) var3) = (*((double*) var1)) / ((double)*((int*) var2));
							}
							else
							{
								if (*((double*) var2) == 0.0)
								{
									//clearall
									printf("Behova chyba, deleni nulou."); //smazat!!
									return 8; //deleni nulou, behova chyba 8
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
						var3 = l_arr[index3];
					else
						var3 = g_arr[index3];

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
						var3 = l_arr[index3];
					else
						var3 = g_arr[index3];

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
				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

							*((bool*) var3) = *((double*) var1) > *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
								memcpy(var1, ((char*) l_arr[index1]), sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
								memcpy(var1, ((char*) g_arr[index1]), sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
								memcpy(var2, ((char*) l_arr[index2]), sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
								memcpy(var2, ((char*) g_arr[index2]), sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

							*((bool*) var3) = *((double*) var1) < *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
								memcpy(var1, ((char*) l_arr[index1]), sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
								memcpy(var1, ((char*) g_arr[index1]), sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
								memcpy(var2, ((char*) l_arr[index2]), sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
								memcpy(var2, ((char*) g_arr[index2]), sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;				

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

							*((bool*) var3) = *((double*) var1) == *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
								memcpy(var1, ((char*) l_arr[index1]), sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
								memcpy(var1, ((char*) g_arr[index1]), sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
								memcpy(var2, ((char*) l_arr[index2]), sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
								memcpy(var2, ((char*) g_arr[index2]), sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

							*((bool*) var3) = *((double*) var1) >= *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
								memcpy(var1, ((char*) l_arr[index1]), sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
								memcpy(var1, ((char*) g_arr[index1]), sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
								memcpy(var2, ((char*) l_arr[index2]), sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
								memcpy(var2, ((char*) g_arr[index2]), sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

							*((bool*) var3) = *((double*) var1) <= *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = malloc(sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
								memcpy(var1, ((char*) l_arr[index1]), sizeof(char) * (strlen(((char*) l_arr[index1])) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
								memcpy(var1, ((char*) g_arr[index1]), sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
								memcpy(var2, ((char*) l_arr[index2]), sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen((char*) g_arr[index2]) + 1));
								memcpy(var2, ((char*) g_arr[index2]), sizeof(char) * (strlen((char*) g_arr[index2]) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
				if ((((htab_item*) I->addr1)->initialized == 0) || (((htab_item*) I->addr2)->initialized == 0))
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				switch(type1)
				{
					case 0:
					{
						if ((type2 == 0) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

							*((bool*) var3) = *((double*) var1) != *((double*) var2);
						}
						break;
					}

					case 3:
					{
						if ((type2 == 3) && (type3 == 3))
						{
							if (scope1 == 0)
								var1 = l_arr[index1];
							else
								var1 = g_arr[index1];

							if (scope2 == 0)
								var2 = l_arr[index2];
							else
								var2 = g_arr[index2];

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
								var1 = malloc(sizeof(char) * (strlen((char*) l_arr[index1]) + 1));
								memcpy(var1, l_arr[index1], sizeof(char) * (strlen((char*) l_arr[index1]) + 1));
							}
							else
							{
								var1 = malloc(sizeof(char) * (strlen((char*) g_arr[index1]) + 1));
								memcpy(var1, ((char*) g_arr[index1]), sizeof(char) * (strlen(((char*) g_arr[index1])) + 1));
							}

							if (scope2 == 0)
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
								memcpy(var2, ((char*) l_arr[index2]), sizeof(char) * (strlen(((char*) l_arr[index2])) + 1));
							}
							else
							{
								var2 = malloc(sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
								memcpy(var2, ((char*) g_arr[index2]), sizeof(char) * (strlen(((char*) g_arr[index2])) + 1));
							}

							if (scope3 == 0)
								var3 = l_arr[index3];
							else
								var3 = g_arr[index3];

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
				((htab_item*) I->addr3)->initialized = 1;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;
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
							*((int*) l_arr[index3]) = *((int*) I->addr1);
						else
							*((int*) g_arr[index3]) = *((int*) I->addr1);
						//free(I->addr1);
						//I->addr1 = NULL;
						break;
					}

					case 1:
					{
						if (scope3 == 0)
							*((double*) l_arr[index3]) = *((double*) I->addr1);
						else
							*((double*) g_arr[index3]) = *((double*) I->addr1);
						//free(I->addr1);
						//I->addr1 = NULL;
						break;
					}

					case 2:
					{
						if (scope3 == 0)
						{
							if (strlen((char*) I->addr1) != strlen((char*) l_arr[index3]))
							{
								free(l_arr[index3]);
								l_arr[index3] = NULL;
								l_arr[index3] = malloc(sizeof(char) * (strlen((char*) I->addr1) + 1));
							}
							memcpy(((char*) l_arr[index3]), ((char*) I->addr1), (strlen((char*) I->addr1) + 1) * sizeof(char));
						}
						else
						{
							if (strlen((char*) I->addr1) != strlen((char*) g_arr[index3]))
							{
								free(g_arr[index3]);
								g_arr[index3] = NULL;
								g_arr[index3] = malloc(sizeof(char) * (strlen((char*) I->addr1) + 1));
							}
							memcpy(((char*) g_arr[index3]), ((char*) I->addr1), (strlen((char*) I->addr1) + 1) * sizeof(char));
						}
						//free(I->addr1);
						//I->addr1 = NULL;
						break;
					}

					case 3:
					{
						if (scope3 == 0)
							*((bool*) l_arr[index3]) = *((bool*) I->addr1);
						else
							*((bool*) g_arr[index3]) = *((bool*) I->addr1);
						//free(I->addr1);
						//I->addr1 = NULL;
						break;
					}
				}
				break;
			}

			case I_COPYVAR:
			{
				if (((htab_item*) I->addr1)->initialized == 0)
				{
					//clearall - nejdriv uvolnit vsechny local array, pak global a vse ostatni
					printf("Neinicializovany operand."); //smazat!!
					return 7; //prace s neinicializovanou promennou, behova chyba 7
				}
				((htab_item*) I->addr3)->initialized = 1;
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;
				
				switch (type1)
				{
					case 0:
					{
						if (scope3 == 0)
						{
							if (scope1 == 0)
								*((int*) l_arr[index3]) = *((int*) l_arr[index1]);
							else
								*((int*) l_arr[index3]) = *((int*) g_arr[index1]);
						}
						else
						{
							if (scope1 == 0)
								*((int*) g_arr[index3]) = *((int*) l_arr[index1]);
							else
								*((int*) g_arr[index3]) = *((int*) g_arr[index1]);
						}
						break;
					}

					case 1:
					{
						if (scope3 == 0)
						{
							if (scope1 == 0)
								*((double*) l_arr[index3]) = *((double*) l_arr[index1]);
							else
								*((double*) l_arr[index3]) = *((double*) g_arr[index1]);
						}
						else
						{
							if (scope1 == 0)
								*((double*) g_arr[index3]) = *((double*) l_arr[index1]);
							else
								*((double*) g_arr[index3]) = *((double*) g_arr[index1]);
						}
						break;
					}

					case 2:
					{
						if (scope3 == 0)
						{
							if (scope1 == 0)
							{
								if (strlen((char*) l_arr[index3]) != strlen((char*) l_arr[index1]))
								{
								free(l_arr[index3]);
								l_arr[index3] = NULL;
								l_arr[index3] = malloc(sizeof(char) * (strlen((char*) l_arr[index1]) + 1));
								}
								memcpy(((char*) l_arr[index3]), ((char*) l_arr[index1]), sizeof(char) * (strlen((char*) l_arr[index1]) + 1));
							}
							else
							{
								if (strlen((char*) l_arr[index3]) != strlen((char*) g_arr[index1]))
								{
									free(l_arr[index3]);
									l_arr[index3] = NULL;
									l_arr[index3] = malloc(sizeof(char) * (strlen((char*) g_arr[index1]) + 1));
								}
								memcpy(((char*) l_arr[index3]), ((char*) g_arr[index1]), sizeof(char) * (strlen((char*) g_arr[index1]) + 1));
							}
						}
						else
						{
							if (scope1 == 0)
							{
								if (strlen((char*) g_arr[index3]) != strlen((char*) l_arr[index1]))
								{
									free(g_arr[index3]);
									g_arr[index3] = NULL;
									g_arr[index3] = malloc(sizeof(char) * (strlen((char*) l_arr[index1]) + 1));
								}
								memcpy(((char*) g_arr[index3]), ((char*) l_arr[index1]), sizeof(char) * (strlen((char*) l_arr[index1]) + 1));
							}
							else
							{
								if (strlen((char*) g_arr[index3]) != strlen((char*) g_arr[index1]))
								{
									free(g_arr[index3]);
									g_arr[index3] = NULL;
									g_arr[index3] = malloc(sizeof(char) * (strlen((char*) g_arr[index1]) + 1));
								}
								memcpy(((char*) g_arr[index3]), ((char*) g_arr[index1]), sizeof(char) * (strlen((char*) g_arr[index1]) + 1));
							}
						}
						break;
					}

					case 3:
					{
						if (scope3 == 0)
						{
							if (scope1 == 0)
								*((bool*) l_arr[index3]) = *((bool*) l_arr[index1]);
							else
								*((bool*) l_arr[index3]) = *((bool*) g_arr[index1]);
						}
						else
						{
							if (scope1 == 0)
								*((bool*) g_arr[index3]) = *((bool*) l_arr[index1]);
							else
								*((bool*) g_arr[index3]) = *((bool*) g_arr[index1]);
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
				*init_temp = ((htab_item*) I->addr3)->initialized;
				//search table for type, index and scope
				//push value from array
				switch (type3)
				{
					case 0:
					{
						if (scope3 == 0)
							VarStackPush(VS, ((int*) l_arr[index3]), NULL, NULL, NULL);
						else
							VarStackPush(VS, ((int*) g_arr[index3]), NULL, NULL, NULL);
						break;
					}

					case 1:
					{
						if (scope3 == 0)
							VarStackPush(VS, NULL, ((double*) l_arr[index3]), NULL, NULL);
						else
							VarStackPush(VS, NULL, ((double*) g_arr[index3]), NULL, NULL);
						break;
					}

					case 2:
					{
						if (scope3 == 0)
							VarStackPush(VS, NULL, NULL, ((char*) l_arr[index3]), NULL);
						else
							VarStackPush(VS, NULL, NULL, ((char*) g_arr[index3]), NULL);
						break;
					}

					case 3:
					{
						if (scope3 == 0)
							VarStackPush(VS, NULL, NULL, NULL, ((bool*) l_arr[index3]));
						else
							VarStackPush(VS, NULL, NULL, NULL, ((bool*) g_arr[index3]));
						break;
					}
				}

				VarStackPush(VS, NULL, NULL, NULL, init_temp);
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
				((htab_item*) I->addr3)->initialized = 1;

				if (l_arr != NULL)
					LStackPush(LS, l_arr);

				l_arr = NULL;
				l_arr = malloc(sizeof(void*) * (((htab_item*) I->addr1)->func_table->item_count));
				initarray(l_arr, ((htab_item*) I->addr1)->func_table->item_count);
				loadarray(l_arr, ((htab_item*) I->addr1)->func_table);

				if ((((htab_item*) I->addr1)->func_data) != NULL)
					i = strlen(((htab_item*) I->addr1)->func_data);
				else
					i = 0;

				while (i > 0)
				{
					char *temp;
					htab_item* Item = FindItemOfIndex(i, ((htab_item*) I->addr1)->func_table);
					Item->initialized = BoolVarStackPop(VS);
					switch (((htab_item*) I->addr1)->func_data[(i-1)]) //cteni znaku retezce typu - strlen ==3 -> posledni znak je na pozici [2]
					{
						case 'i':
						{
							*((int*) l_arr[i]) = IntVarStackPop(VS);
							break;
						}

						case 's':
						{
							temp = StrVarStackPop(VS);
							if (strlen(temp) != strlen((char*) l_arr[i]))
							{
								free(l_arr[i]);
								l_arr[i] = NULL;
								l_arr[i] = malloc(sizeof(char) * (strlen(temp) + 1));
							}
							memcpy(((char*) l_arr[i]), temp, sizeof(char) * (strlen(temp) + 1));
							//free(temp);
							break;	
						}

						case 'r':
						{
							*((double*) l_arr[i]) = DoubleVarStackPop(VS);
							break;
						}

						case 'b':
						{
							*((bool*) l_arr[i]) = BoolVarStackPop(VS);
							break;
						}
					}
					i--;
				}
				
				
				VarStackPush(VS, index_temp, NULL, NULL, NULL);
				VarStackPush(VS, NULL, NULL, NULL, scope_temp);
				VarStackPush(VS, size_temp, NULL, NULL, NULL);
				VarStackPush(VS, type_temp, NULL, NULL, NULL);
				if (return_addr != NULL)
					AddStackPush(AS, return_addr);
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
						*((int*) value_temp) = *((int*) l_arr[0]);
						break;
					}

					case 1:
					{
						value_temp = malloc(sizeof(double));
						*((double*) value_temp) = *((double*) l_arr[0]);
						break;
					}

					case 2:
					{
						value_temp = malloc(sizeof(char) * (strlen((char*) l_arr[0]) + 1));
						memcpy(((char*) value_temp), ((char*) l_arr[0]), sizeof(char) * (strlen((char*) l_arr[0]) + 1));
						break;
					}

					case 3:
					{
						value_temp = malloc(sizeof(bool));
						*((bool*) value_temp) = *((bool*) l_arr[0]);
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
							*((int*) l_arr[*index_temp]) = *((int*) value_temp);
							break;
						}

						case 1:
						{
							*((double*) l_arr[*index_temp]) = *((double*) value_temp);
							break;
						}

						case 2:
						{
							if ((strlen((char*) value_temp)) != strlen((char*) l_arr[*index_temp]))
							{
								free(l_arr[*index_temp]);
								l_arr[*index_temp] = NULL;
								l_arr[*index_temp] = malloc(sizeof(char) * (strlen((char*) value_temp) + 1));
							}
							memcpy(((char*) l_arr[*index_temp]), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
							break;
						}

						case 3:
						{
							*((bool*) l_arr[*index_temp]) = *((bool*) value_temp);
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
							*((int*) g_arr[*index_temp]) = *((int*) value_temp);
							break;
						}

						case 1:
						{
							*((double*) g_arr[*index_temp]) = *((double*) value_temp);
							break;
						}

						case 2:
						{
							if ((strlen((char*) value_temp)) != strlen((char*) g_arr[*index_temp]))
							{
								free(g_arr[*index_temp]);
								g_arr[*index_temp] = NULL;
								g_arr[*index_temp] = malloc(sizeof(char) * (strlen((char*) value_temp) + 1));
							}
							memcpy(((char*) g_arr[*index_temp]), ((char*) value_temp), (sizeof(char) * (strlen((char*) value_temp) + 1)));
							break;
						}

						case 3:
						{
							*((bool*) g_arr[*index_temp]) = *((bool*) value_temp);
							break;
						}
					}
				}

				free(value_temp);
				GoToItem(List, return_addr);
				return_addr = AddStackPop(AS);				
				break;
			}
		}
		NextIns(List);
	}
}
