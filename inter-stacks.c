//zásobníky
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
#include "inter-stacks.h"

//---------------------------------------------------------
//LOCAL VARIABLE ARRAY STACK

void LStackInit (tLVS *S, int StackSize)
{
	S->top = 0;
	S->StackSize = StackSize;
}

int LStackEmpty (tLVS *S)
{
	return (S->top == 0);
}

void LStackPush (tLVS *S, vararr Array)
{
	if (S->top == S->StackSize)
	{
		if ((realloc(S, S->StackSize + sizeof(tLVS))) == NULL)
		{
			printf("malo mista");
			return;
		}
		S->StackSize++;
	}
	else 
	{
		S->top++;  
		S->l_stack[S->top]=Array;
	}
}

vararr LStackPop (tLVS *S)
{
	if (S->top == 0)
	{
		printf("Stack is empty!");
		return NULL;
	}
	else
		return (S->l_stack[S->top--]);
}


//------------------------------------------------------------
//VARIABLE STACK - na parametry a jine promenne

void VarStackInit (tVarS *S, int StackSize)
{
	S->top = 0;
	S->StackSize = StackSize;
}

int VarStackEmpty (tVarS *S)
{
	return (S->top == 0);
}

void VarStackPush (tVarS *S, int *intvar, double *doublevar, char *stringvar, bool *boolvar)
{
	if (S->top == S->StackSize)
	{
		if ((realloc(S, S->StackSize + sizeof(tVarS))) == NULL)
		{
			printf("malo mista\n");
			return;
		}
		S->StackSize++;
	}
	if (intvar != NULL)
	{
		S->top++;
		//S->var_stack[S->top].type = 0;
		(S->var_stack[S->top]).variable = malloc(sizeof(int));
		*((int*)((S->var_stack[S->top]).variable)) = *intvar;
	}
	else if (doublevar != NULL)
	{
		S->top++;
		//S->var_stack[S->top].type = 1;
		(S->var_stack[S->top]).variable = malloc(sizeof(double));
		*((double*)((S->var_stack[S->top]).variable)) = *doublevar;
	}
	else if (stringvar != NULL)
	{
		S->top++;
		//S->var_stack[S->top].type = 2;
		int size = strlen(stringvar) + 1;
		(S->var_stack[S->top]).variable = malloc(sizeof(char) * size);
		memcpy((S->var_stack[S->top]).variable, stringvar, size);
	}
	else if (boolvar!= NULL)
	{
		S->top++;
		(S->var_stack[S->top]).variable = malloc(sizeof(bool));
		*((bool*)((S->var_stack[S->top]).variable)) = *boolvar;
	}
}

int IntVarStackPop (tVarS *S)
{
	int a;
	if (S->top == 0)
	{
		printf("Stack is empty!\n\n");
		return -1;
	}
	else
	{
		a = *((int*)((S->var_stack[S->top--]).variable));
		free((S->var_stack[S->top]).variable);
		return a;
	}
}

bool BoolVarStackPop (tVarS *S)
{
	bool a;
	if (S->top == 0)
	{
		printf("Stack is empty!\n\n");
		return -1;
	}
	else
	{
		a = *((bool*)((S->var_stack[S->top--]).variable));
		free((S->var_stack[S->top]).variable);
		return a;
	}
}

double DoubleVarStackPop (tVarS *S)
{
	if (S->top == 0)
	{
		printf("Stack is empty!\n\n");
		return -1.0;
	}
	else
		return *((double*)((S->var_stack[S->top--]).variable));
}

char* StrVarStackPop (tVarS *S)
{
	if (S->top == 0)
	{
		printf("Stack is empty!\n\n");
		return NULL;
	}
	else
		return ((char*)((S->var_stack[S->top--]).variable));
}

//------------------------------------------------------------
//RETURN ADDRESS STACK

void AddStackInit (tAddS *S, int StackSize)
{
	S->top = 0;
	S->StackSize = StackSize;
}

int AddStackEmpty (tAddS *S)
{
	return (S->top == 0);
}

void AddStackPush (tAddS *S, void *item)
{
	if (S->top == S->StackSize)
	{
		if ((realloc(S, S->StackSize + sizeof(tAddS))) == NULL)
		{
			printf("malo mista");
			return;
		}
		S->StackSize++;
	}
	else 
	{
		S->top++;  
		S->add_stack[S->top] = item;
	}
}

void* AddStackPop (tAddS *S)
{
	if (S->top == 0)
	{
		printf("Stack is empty!");
		return NULL;
	}
	else
		return (S->add_stack[S->top--]);
}