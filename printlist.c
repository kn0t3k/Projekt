#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "printlist.h"

void InitPrintList (tPrintList *L)
{
	L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;
}

void InsertPrintNew (tPrintList *L, int type, void *var)
{
	tPrintItem *NewItem;

	if ((NewItem = malloc(sizeof(tPrintItem))) == NULL)
			printf("nealokovalo se\n");
	NewItem->var = var;
	NewItem->type = type;
	NewItem->NextItem = NULL;
	if (L->First != NULL)
	{
		NewItem->NextItem = L->First;
		L->First = NewItem;
	}
	else
	{
		L->Last = NewItem;
		L->First = NewItem;
	}
}

void PrintAll (tPrintList *L)
{
	L->Act = L->First;
	while (L->Act != NULL)
	{
		switch (L->Act->type)
		{
			case 0:
			{
				printf("%d", *((int*) L->Act->var));
				break;
			}

			case 1:
			{
				printf("%g", *((double*) L->Act->var));
				break;
			}

			case 2:
			{
				printf("%s", ((char*) L->Act->var));
			}

			case 3:
			{
				printf("%d", *((bool*) L->Act->var));
			}
		}
		L->Act = L->Act->NextItem;
	}
	DisposePrintList(L);
}

void DisposePrintList (tPrintList *L)
{
	if (L->First != NULL)
	{
		while (L->First != NULL)
		{
			L->Act = L->First;
			L->First = L->First->NextItem;
			free(L->Act->var);
			free(L->Act);
		}
		L->First = NULL;
		L->Last = NULL;
		L->Act = NULL;
	}
}
