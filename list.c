/*--------------------------------------
| Projekt: IFJ14						
| Soubor: list.c						
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
#include "list.h"

#define INTERNAL_ERR -99

void InitList (tList *L)
{
	L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;
	L->Top = NULL;
}

void SetFirst (tList *L, void *ItemAddress)
{
	L->First = (tItem*) ItemAddress;
}

void DisposeList (tList *L)
{
	if (L->Top != NULL)
	{
		while (L->Top != NULL)
		{
			L->Act = L->Top;
			L->Top = L->Top->NextItem;
			free(L->Act);
		}
		L->Top = NULL;
		L->First = NULL;
		L->Last = NULL;
		L->Act = NULL;
	}
}

int InsertNew (tList *L, tInstr NewIns)
{
	struct Item *NewItem;

	if ((NewItem = malloc(sizeof(struct Item))) == NULL)
			return INTERNAL_ERR;
	NewItem->Instruction = NewIns;
	NewItem->NextItem = NULL;
	if (L->Last != NULL)
	{
		L->Last->NextItem = NewItem;
		L->Last = NewItem;
	}
	else
	{
		L->Top = NewItem;
		L->Last = NewItem;
		L->First = NewItem;
	}
	return 0;
}

void *LastItemAddress (tList *L)
{
	return ((void*) L->Last);
}

void *CurrentItemAddress (tList *L)
{
	return ((void*) L->Act);
}

void GoToItem (tList *L, void *ItemAddress)
{
	L->Act = (tItem*) ItemAddress;
}

void First (tList *L)
{
	L->Act = L->First;
}

void Last (tList *L)
{
	L->Act = L->Last;	
}

void NextIns (tList *L)
{
	if (L->Act != NULL)
		L->Act = L->Act->NextItem;
}

tInstr *GetData (tList *L)
{
	if (L->Act == NULL)
		return NULL;
	return &(L->Act->Instruction);
}

