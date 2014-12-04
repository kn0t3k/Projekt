#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "interpret.h"
#include "inter-stacks.h"
#include "err.h"

#define VAR_STACK_SIZE 10
#define ADS_STACK_SIZE 10
#define LVS_STACK_SIZE 10

void loadarray(void **array, symbol_table_item *TB)
{
	int hash_size = TB->table->htab_size;
	htab_item *iptr = NULL;

	for (int i = 0; i < hash_size; i++)
	{
		iptr = TB->table->ptr[i];
		while (iptr != NULL)
		{
			int type = iptr->type;
			switch (type)
			{
				case INT_INT:
					array[iptr->index] = malloc(sizeof(int));
					break;

				case INT_REAL:
					array[iptr->index] = malloc(sizeof(double));
					break;

				case INT_STRING:
					array[iptr->index] = malloc(sizeof(char));
					break;

				case INT_BOOLEAN:
					array[iptr->index] = malloc(sizeof(bool));
					break;

				default:
					break;
			}
			iptr = iptr->next;
		}
	}
} 

void disposearray(void **array, int size)
{
	for (int i = 0; i < size; i++)
		free(array[i]);

	free(array);
}

int interpret(symbol_table_item *GTable, tList *L)
{
	First(L);
	tInstr *I;
	int *index_temp = malloc(sizeof(int));
	int *size_temp = malloc(sizeof(int));
	int *type_temp = malloc(sizeof(int));
	bool *scope_temp = malloc(sizeof(bool));
	void *value_temp;
	void *return_addr = NULL;
	int index1 = -1;
	int index2 = -1;
	int index3 = -1;
	int type1 = -1;
	int type2 = -1;
	int type3 = -1;
	bool scope1 = -1; //pro scope plati: 0 == local, 1 == global
	bool scope2 = -1;
	bool scope3 = -1;
	void *var1 = NULL;
	void *var2 = NULL;
	void *var3 = NULL;
	tVarS *VS = malloc(sizeof(tVarS)); //variable stack
	VS->var_stack = malloc(sizeof(void*) * (VAR_STACK_SIZE));
	VarStackInit(VS,VAR_STACK_SIZE);
	void **g_arr = malloc(sizeof(void*) * GTable->item_count);
	loadarray(g_arr, GTable);

	tLVS *LS = malloc(sizeof(tLVS)); //local array stack
	LS->l_stack = malloc(sizeof(vararr) * (LVS_STACK_SIZE));
	LStackInit(LS, LVS_STACK_SIZE);
	void **l_arr = NULL;

	tAddS *AS = malloc(sizeof(tAddS));
	AS->add_stack = malloc(sizeof(tItem)*(ADS_STACK_SIZE));
	AddStackInit(AS, ADS_STACK_SIZE);

//pri ruseni velikost u funkce v tabulce
	
//load global array
//initiate stacks
	while (1)
	{
		I = GetData(L);
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
				if (type1 != INT_STRING)
				{
					if (scope1 == 0)
					{
						if (!(l_arr[index1]))
							GoToItem(L,((tItem*) I->addr3)); //melo by fungovat...
					}
					else
					{
						if (!(g_arr[index1]))
							GoToItem(L,((tItem*) I->addr3));
					}
				}
				break;
			}

			case I_GOTO:
			{
				GoToItem(L,((tItem*) I->addr3));
				break;
			}

			case I_END:
			{
				disposearray(g_arr, GTable->item_count);
				return 0;
			}			

			//instrukce nacteni a zapsani
			//case I_READ:
			case I_WRITE:
			{
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				switch (type3)
				{
					case INT_INT:
						if (scope3 == 0)
							printf("%d\n", *((int*) l_arr[index3]));
						else
							printf("%d\n", *((int*) g_arr[index3]));
						break;

					case INT_REAL:
						if (scope3 == 0)
							printf("%g\n", *((double*) l_arr[index3]));
						else
							printf("%g\n", *((double*) g_arr[index3]));
						break;

					case INT_STRING:
						if (scope3 == 0)
							printf("%s\n", ((char*) l_arr[index3]));
						else
							printf("%s\n", ((char*) g_arr[index3]));
						break;

					case INT_BOOLEAN:
						if (scope3 == 0)
							printf("%d\n", *((bool*) l_arr[index3]));
						else
							printf("%d\n", *((bool*) g_arr[index3]));
				}	
				break;
			}

			//nase instrukce
			/*case I_FIND:
			case I_SORT:
			case I_LENGTH:
			case I_COPY:*/

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

				if ((type1 != INT_STRING) && (type2 != INT_STRING) && (type3 != INT_STRING))
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

					if (type1 == INT_INT)
					{
						if (type2 == INT_INT)
							*((int*) var3) = *((int*) var1) + *((int*) var2);
						else
							*((double*) var3) = ((double)*((int*) var1)) + *((double*) var2);
					}
					else 
					{
						if (type1 == INT_REAL)
						{
							if (type2 == INT_INT)
								*((double*) var3) = *((double*) var1) + ((double)(*((int*) var2)));
							else
								*((double*) var3) = *((double*) var1) + *((double*) var2);
						}
					}
				}
				else if ((type1 == INT_STRING) && (type2 == INT_STRING) && (type3 == INT_STRING))
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
						if (strlen(((char*) var3)) > strlen(((char*) l_arr[index3])))
							if ((realloc(l_arr[index3], (strlen(((char*) var3)) + 1) * sizeof(char))) == NULL)
								printf("chyba!!");
						memcpy(((char*) l_arr[index3]), ((char*) var3), (strlen(((char*) var3)) + 1) * sizeof(char));
					}
					else
					{
						if (strlen(((char*) var3)) > strlen(((char*) g_arr[index3])))
							if ((realloc(g_arr[index3], (strlen(((char*) var3)) + 1) * sizeof(char))) == NULL)
								printf("chyba!!");
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
				index1 = ((htab_item*) I->addr1)->index;
				type1 = ((htab_item*) I->addr1)->type;
				scope1 = ((htab_item*) I->addr1)->global;
				index2 = ((htab_item*) I->addr2)->index;
				type2 = ((htab_item*) I->addr2)->type;
				scope2 = ((htab_item*) I->addr2)->global;
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if ((type1 != INT_STRING) && (type2 != INT_STRING) && (type3 != INT_STRING))
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

					if (type1 == INT_INT)
					{
							if (type2 == INT_INT)
								*((int*) var3) = *((int*) var1) - *((int*) var2);
							else
								*((double*) var3) = ((double)*((int*) var1)) - *((double*) var2);
					}
					else 
					{
						if (type1 == INT_REAL)
						{
							if (type2 == INT_INT)
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

				if ((type1 != INT_STRING) && (type2 != INT_STRING) && (type3 != INT_STRING))
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

					if (type1 == INT_INT)
					{
							if (type2 == INT_INT)
								*((int*) var3) = (*((int*) var1)) * (*((int*) var2));
							else
								*((double*) var3) = ((double)*((int*) var1)) * (*((double*) var2));
					}
					else 
					{
						if (type1 == INT_REAL)
						{
							if (type2 == INT_INT)
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

				if ((type1 != INT_STRING) && (type2 != INT_STRING) && (type3 != INT_STRING))
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

					if (type1 == INT_INT)
					{
						if (type2 == INT_INT)
							*((double*) var3) = ((double)*((int*) var1)) / ((double)(*((int*) var2)));
						else
							*((double*) var3) = ((double)*((int*) var1)) / (*((double*) var2));
					}
					else
					{
						if (type1 == INT_REAL)
						{
							if (type2 == INT_INT)
								*((double*) var3) = (*((double*) var1)) / ((double)*((int*) var2));
							else
								*((double*) var3) = (*((double*) var1)) / (*((double*) var2));
						}
					}
				}
				break;
			}

			case I_INC: //++
			{
				index3 = ((htab_item*) I->addr3)->index;
				type3 = ((htab_item*) I->addr3)->type;
				scope3 = ((htab_item*) I->addr3)->global;

				if (type3 != INT_STRING)
				{
					if (scope3 == 0)
						var3 = l_arr[index3];
					else
						var3 = g_arr[index3];

					if (type3 == INT_INT)
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

				if (type3 != INT_STRING)
				{
					if (scope3 == 0)
						var3 = l_arr[index3];
					else
						var3 = g_arr[index3];

					if (type3 == INT_INT)
						(*((int*) var3))--;
					else
						(*((double*) var3))--;
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

				switch(type1)
				{
					case INT_INT:
					{
						if ((type2 == INT_INT) && (type3 == INT_BOOLEAN))
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

					case INT_REAL:
					{
						if ((type2 == INT_REAL) && (type3 == INT_BOOLEAN))
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

					case INT_BOOLEAN:
					{
						if ((type2 == INT_BOOLEAN) && (type3 == INT_BOOLEAN))
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

					case INT_STRING:
					{
						if ((type2 == INT_STRING) && (type3 == INT_BOOLEAN))
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
					case INT_INT:
					{
						if ((type2 == INT_INT) && (type3 == INT_BOOLEAN))
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

					case INT_REAL:
					{
						if ((type2 == INT_REAL) && (type3 == INT_BOOLEAN))
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

					case INT_BOOLEAN:
					{
						if ((type2 == INT_BOOLEAN) && (type3 == INT_BOOLEAN))
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

					case INT_STRING:
					{
						if ((type2 == INT_STRING) && (type3 == INT_BOOLEAN))
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
					case INT_INT:
					{
						if ((type2 == INT_INT) && (type3 == INT_BOOLEAN))
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

					case INT_REAL:
					{
						if ((type2 == INT_REAL) && (type3 == INT_BOOLEAN))
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

					case INT_BOOLEAN:
					{
						if ((type2 == INT_BOOLEAN) && (type3 == INT_BOOLEAN))
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

					case INT_STRING:
					{
						if ((type2 == INT_STRING) && (type3 == INT_BOOLEAN))
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
					case INT_INT:
					{
						if ((type2 == INT_INT) && (type3 == INT_BOOLEAN))
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

					case INT_REAL:
					{
						if ((type2 == INT_REAL) && (type3 == INT_BOOLEAN))
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

					case INT_BOOLEAN:
					{
						if ((type2 == INT_BOOLEAN) && (type3 == INT_BOOLEAN))
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

					case INT_STRING:
					{
						if ((type2 == INT_STRING) && (type3 == INT_BOOLEAN))
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
					case INT_INT:
					{
						if ((type2 == INT_INT) && (type3 == INT_BOOLEAN))
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

					case INT_REAL:
					{
						if ((type2 == INT_REAL) && (type3 == INT_BOOLEAN))
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

					case INT_BOOLEAN:
					{
						if ((type2 == INT_BOOLEAN) && (type3 == INT_BOOLEAN))
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

					case INT_STRING:
					{
						if ((type2 == INT_STRING) && (type3 == INT_BOOLEAN))
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
					case INT_INT:
					{
						if ((type2 == INT_INT) && (type3 == INT_BOOLEAN))
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

					case INT_REAL:
					{
						if ((type2 == INT_REAL) && (type3 == INT_BOOLEAN))
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

					case INT_BOOLEAN:
					{
						if ((type2 == INT_BOOLEAN) && (type3 == INT_BOOLEAN))
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

					case INT_STRING:
					{
						if ((type2 == INT_STRING) && (type3 == INT_BOOLEAN))
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
					case INT_INT:
					{
						if (scope3 == 0)
							*((int*) l_arr[index3]) = *((int*) I->addr1);
						else
							*((int*) g_arr[index3]) = *((int*) I->addr1);
						free(I->addr1);
						break;
					}

					case INT_REAL:
					{
						if (scope3 == 0)
							*((double*) l_arr[index3]) = *((double*) I->addr1);
						else
							*((double*) g_arr[index3]) = *((double*) I->addr1);
						free(I->addr1);
						break;
					}

					case INT_STRING:
					{
						if (scope3 == 0)
						{
							if (strlen((char*) I->addr1) > strlen((char*) l_arr[index3]))
								if ((realloc(((char*) l_arr[index3]), (strlen((char*) I->addr1) + 1) * sizeof(char))) == NULL)
									printf("chyba!!");
							memcpy(((char*) l_arr[index3]), ((char*) I->addr1), (strlen((char*) I->addr1) + 1) * sizeof(char));
						}
						else
						{
							if (strlen((char*) I->addr1) > strlen((char*) g_arr[index3]))
								if ((realloc(((char*) g_arr[index3]), (strlen((char*) I->addr1) + 1) * sizeof(char))) == NULL)
									printf("chyba!!");
							memcpy(((char*) g_arr[index3]), ((char*) I->addr1), (strlen((char*) I->addr1) + 1) * sizeof(char));
						}
						free(I->addr1);
						break;
					}

					case INT_BOOLEAN:
					{
						if (scope3 == 0)
							*((bool*) l_arr[index3]) = *((bool*) I->addr1);
						else
							*((bool*) g_arr[index3]) = *((bool*) I->addr1);
						free(I->addr1);
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
				//search table for type, index and scope
				//push value from array
				switch (type3)
				{
					case INT_INT:
					{
						if (scope3 == 0)
							VarStackPush(VS, ((int*) l_arr[index3]), NULL, NULL, NULL);
						else
							VarStackPush(VS, ((int*) g_arr[index3]), NULL, NULL, NULL);
						break;
					}

					case INT_REAL:
					{
						if (scope3 == 0)
							VarStackPush(VS, NULL, ((double*) l_arr[index3]), NULL, NULL);
						else
							VarStackPush(VS, NULL, ((double*) g_arr[index3]), NULL, NULL);
						break;
					}

					case INT_STRING:
					{
						if (scope3 == 0)
							VarStackPush(VS, NULL, NULL, ((char*) l_arr[index3]), NULL);
						else
							VarStackPush(VS, NULL, NULL, ((char*) g_arr[index3]), NULL);
						break;
					}

					case INT_BOOLEAN:
					{
						if (scope3 == 0)
							VarStackPush(VS, NULL, NULL, NULL, ((bool*) l_arr[index3]));
						else
							VarStackPush(VS, NULL, NULL, NULL, ((bool*) g_arr[index3]));
						break;
					}
				}
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
				//get new return address - adress current instruction (after return, NextIns(L) is used
				//goto addr1

				*size_temp = ((htab_item*) I->addr1)->func_table->item_count;
				*type_temp = ((htab_item*) I->addr3)->type;
				*index_temp = ((htab_item*) I->addr3)->index;
				*scope_temp = ((htab_item*) I->addr3)->global;

				if (l_arr != NULL)
					LStackPush(LS, l_arr);

				l_arr = NULL;
				l_arr = malloc(sizeof(void*) * (((htab_item*) I->addr1)->func_table->item_count));
				loadarray(l_arr, ((htab_item*) I->addr1)->func_table);

				for (int i = strlen(((htab_item*) I->addr1)->func_data); i > 0; i--)
				{
					char *temp;
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
							memcpy(((char*) l_arr[i]), temp, sizeof(char) * (strlen(temp) + 1));
							free(temp);
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
				}
				
				
				VarStackPush(VS, index_temp, NULL, NULL, NULL);
				VarStackPush(VS, NULL, NULL, NULL, scope_temp);
				VarStackPush(VS, size_temp, NULL, NULL, NULL); //musi byt prvni co popnu v return
				VarStackPush(VS, type_temp, NULL, NULL, NULL);
				if (return_addr != NULL)
					AddStackPush(AS, return_addr);
				return_addr = CurrentItemAddress(L);
				
				//GoToItem(L, ((htab_item*) I->addr1)->label); ----!!!!! ODKOMENTOVAT AZ TOMAS DOPLNI !!!!!----
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
					case INT_INT:
					{
						value_temp = malloc(sizeof(int));
						*((int*) value_temp) = *((int*) l_arr[0]);
						break;
					}

					case INT_REAL:
					{
						value_temp = malloc(sizeof(double));
						*((double*) value_temp) = *((double*) l_arr[0]);
						break;
					}

					case INT_STRING:
					{
						value_temp = malloc(sizeof(char) * (strlen((char*) l_arr[0]) + 1));
						memcpy(((char*) value_temp), ((char*) l_arr[0]), sizeof(char) * (strlen((char*) l_arr[0]) + 1));
						break;
					}

					case INT_BOOLEAN:
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
						case INT_INT:
						{
							*((int*) l_arr[*index_temp]) = *((int*) value_temp);
							break;
						}

						case INT_REAL:
						{
							*((double*) l_arr[*index_temp]) = *((double*) value_temp);
							break;
						}

						case INT_STRING:
						{
							if ((strlen((char*) value_temp)) != strlen((char*) l_arr[*index_temp]))
								if ((realloc(((char*) l_arr[*index_temp]), sizeof(char) * (strlen((char*) value_temp) + 1))) == NULL)
									printf("chyba!!");
							memcpy(((char*) l_arr[*index_temp]), ((char*) value_temp), sizeof(char) * (strlen((char*) value_temp) + 1));
							break;
						}

						case INT_BOOLEAN:
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
						case INT_INT:
						{
							*((int*) g_arr[*index_temp]) = *((int*) value_temp);
							break;
						}

						case INT_REAL:
						{
							*((double*) g_arr[*index_temp]) = *((double*) value_temp);
							break;
						}

						case INT_STRING:
						{
							if ((strlen((char*) value_temp)) != strlen((char*) g_arr[*index_temp]))
								if ((realloc(((char*) g_arr[*index_temp]), sizeof(char) * (strlen((char*) value_temp) + 1))) == NULL)
									printf("chyba!!");
							memcpy(((char*) g_arr[*index_temp]), ((char*) value_temp), (sizeof(char) * (strlen((char*) value_temp) + 1)));
							break;
						}

						case INT_BOOLEAN:
						{
							*((bool*) g_arr[*index_temp]) = *((bool*) value_temp);
							break;
						}
					}
				}

				free(value_temp);
				GoToItem(L, return_addr);
				return_addr = AddStackPop(AS);				
				break;
			}
		}
		NextIns(L);
	}
	//!!!! UVOLNIT UKAZATELE !!!!
}
