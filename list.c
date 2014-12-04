#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void InitList (tList *L)
{
	L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;
}

void DisposeList (tList *L)
{
	if (L->First != NULL) //Je potreba?
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

void InsertNew (tList *L, tInstr NewIns)
{
	struct Item *NewItem;

	NewItem = malloc(sizeof(struct Item)); //SELHANI!!!
	NewItem->Instruction = NewIns;
	NewItem->NextItem = NULL;
	if (L->Last != NULL)
	{
		L->Last->NextItem = NewItem;
		L->Last = NewItem;
	}
	else
	{
		L->Last = NewItem;
		L->First = NewItem;
	}
}

void *LastItemAddress (tList *L) //musi vracet void aby se pak dala ta adresa vlozit do 3. adresy instrukce CALL a IFGOTO
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
	if (L->Act != NULL) //Je potreba??
		L->Act = L->Act->NextItem;
}

tInstr *GetData (tList *L)
{
	if (L->Act == NULL) //Je potreba??
		return NULL;
	return &(L->Act->Instruction);
}

