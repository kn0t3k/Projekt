/*--------------------------------------
| Projekt: IFJ14
| Soubor: printlist.c
| Autori: Denk Filip (xdenkf00)
|	  Jurica Tomas (xjuric22)
|	  Knotek Martin (xknote11)
|	  Kohut Jan (xkohut08)
|	  Piskula David (xpisku02)
| Prekladac: GCC 4.8.2-7
| Datum:   5.12.2014
|--------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "printlist.h"
#define INTERNAL_ERR -99

void InitPrintList (tPrintList *L)
{
	L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;
}

void DisposePrintList (tPrintList *L)
{
	if (L->First != NULL)
	{
		while (L->First != NULL)
		{
			L->Act = L->First;
			L->First = L->First->NextItem;
			free(L->Act);
		}
		L->First = NULL;
		L->Last = NULL;
		L->Act = NULL;
	}
}

int InsertPrintNew (tPrintList *L, int type, void *var)
{
	tPrintItem *NewItem;

	if ((NewItem = malloc(sizeof(tPrintItem))) == NULL)
			return INTERNAL_ERR;
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
	return 0;
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
				break;
			}

			case 3:
			{
				if (*((bool*) L->Act->var) == 0)
					printf("FALSE");
				else
					printf("TRUE");
				break;
			}
		}
		L->Act = L->Act->NextItem;
	}
	DisposePrintList(L);
}
