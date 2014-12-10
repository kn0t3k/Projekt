//zásobníky
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
#include "inter-stacks.h"

#define INTERNAL_ERR -99

//---------------------------------------------------------
//LOCAL VARIABLE ARRAY STACK

void LStackInit (tLVS *S, int StackSize)
{
	S->top = 0;
	S->StackSize = StackSize;
	for (int i = 0; i < StackSize; i++)
		S->l_stack[i] = NULL;
}

int LStackEmpty (tLVS *S)
{
	return (S->top == 0);
}

int LStackPush (tLVS *S, vararr Array)
{
	vararr *temp;
	int new_size = 0;
	S->top++;
	if (S->top == S->StackSize)
	{
		new_size = S->StackSize + 20;
		temp = malloc(sizeof(vararr) * new_size);
		if (temp == NULL)
			return INTERNAL_ERR;
		for (int i = 0; i < S->StackSize; i++)
			temp[i] = S->l_stack[i];
		for (int i = S->StackSize; i < new_size; i++)
			temp[i] = NULL;
		free(S->l_stack);
		S->l_stack = temp;
		S->StackSize = new_size;
	} 
	S->l_stack[S->top]=Array;
	return 0;
}

vararr LStackPop (tLVS *S)
{
	if (S->top == 0)
	{
		printf("Stack is empty!1");
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
	for (int i = 0; i < StackSize; i++)
		S->var_stack[i] = NULL;
}

int VarStackEmpty (tVarS *S)
{
	return (S->top == 0);
}

int VarStackPush (tVarS *S, int *intvar, double *doublevar, char *stringvar, bool *boolvar)
{
	void **temp;
	int new_size = 0;
	S->top++;
	if (S->top == S->StackSize)
	{
		new_size = S->StackSize + 20;
		temp = malloc(sizeof(void*) * new_size);
		if (temp == NULL)
			return INTERNAL_ERR;
		for (int i = 0; i < S->StackSize; i++)
			temp[i] = S->var_stack[i];
		for (int i = S->StackSize; i < new_size; i++)
			temp[i] = NULL;
		free(S->var_stack);
		S->var_stack = temp;
		S->StackSize = new_size;	
	}
	if (((S->var_stack[S->top])) != NULL)
	{
		free((S->var_stack[S->top]));
		(S->var_stack[S->top]) = NULL;
	}
	if (intvar != NULL)
	{
		S->var_stack[S->top] = malloc(sizeof(int));
		if (S->var_stack[S->top] == NULL)
			return INTERNAL_ERR;
		*((int*)(S->var_stack[S->top])) = *intvar;
	}
	else if (doublevar != NULL)
	{
		S->var_stack[S->top] = malloc(sizeof(double));
		if (S->var_stack[S->top] == NULL)
			return INTERNAL_ERR;
		*((double*)(S->var_stack[S->top])) = *doublevar;
	}
	else if (stringvar != NULL)
	{
		int size = strlen(stringvar) + 1;
		S->var_stack[S->top] = malloc(sizeof(char) * size);
		if (S->var_stack[S->top] == NULL)
			return INTERNAL_ERR;
		memcpy((S->var_stack[S->top]), stringvar, size);
	}
	else if (boolvar!= NULL)
	{
		S->var_stack[S->top] = malloc(sizeof(bool));
		if (S->var_stack[S->top] == NULL)
			return INTERNAL_ERR;
		*((bool*)(S->var_stack[S->top])) = *boolvar;
	}
	return 0;
}

int IntVarStackPop (tVarS *S)
{
	if (S->top == 0)
	{
		printf("Stack is empty!2\n");
		return -1;
	}
	else
		return *((int*)(S->var_stack[S->top--]));
}

bool BoolVarStackPop (tVarS *S)
{
	if (S->top == 0)
	{
		printf("Stack is empty!3\n");
		return 0;
	}
	else
		return *((bool*)(S->var_stack[S->top--]));
}

double DoubleVarStackPop (tVarS *S)
{
	if (S->top == 0)
	{
		printf("Stack is empty!4\n");
		return -1.0;
	}
	else
		return *((double*)(S->var_stack[S->top--]));
}

char* StrVarStackPop (tVarS *S)
{
	if (S->top == 0)
	{
		printf("Stack is empty!5\n");
		return NULL;
	}
	else
		return ((char*)(S->var_stack[S->top--]));
}

void DisposeVarStack(tVarS *S)
{
	for (int i = 0; i < S->StackSize; i++)
	{
		if (S->var_stack[i] != NULL)
			free(S->var_stack[i]);
		S->var_stack[i] = NULL;
	}
}

//------------------------------------------------------------
//RETURN ADDRESS STACK

void AddStackInit (tAddS *S, int StackSize)
{
	S->top = 0;
	S->StackSize = StackSize;
	for (int i = 0; i < StackSize; i++)
		S->add_stack[i] = NULL;
}

int AddStackEmpty (tAddS *S)
{
	return (S->top == 0);
}

int AddStackPush (tAddS *S, void *item)
{
	void **temp;
	int new_size = 0;
	S->top++; 
	if (S->top == S->StackSize)
	{
		new_size = S->StackSize + 20;
		temp = malloc(sizeof(void*) * new_size);
		if (temp == NULL)
			return INTERNAL_ERR;
		for (int i = 0; i < S->StackSize; i++)
			temp[i] = S->add_stack[i];
		for (int i = S->StackSize; i < new_size; i++)
			temp[i] = NULL;
		free(S->add_stack);
		S->add_stack = temp;
		S->StackSize = new_size;
	} 
	S->add_stack[S->top] = item;
	return 0;
}

void* AddStackPop (tAddS *S)
{
	if (S->top == 0)
		return NULL;
	else
		return (S->add_stack[S->top--]);
}
