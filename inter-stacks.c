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
/*


int main ()
{

		

	return 0;
}
*/

/*vararr NewArray (vararr array, int size)
{
	array = malloc(sizeof(void*) * size);
	for (int i = 0; i < size; i++)
	{
		array[i] = NULL;
	}
	return array;
}*/




/*vararr array = NewArray(array, 5);

	tLVS *stack = malloc(sizeof(tLVS));
	stack->l_stack = malloc(sizeof(vararr)*StackSize);
	LStackInit(stack,StackSize);
	if (LStackEmpty(stack))
		printf("stack is empty -- correct\n\n");
	else
		printf("stack isn't empty -- wrong\n\n");
	LStackPush(stack, &array);
	array = NULL;
	if (LStackEmpty(stack))
		printf("stack is empty -- wrong\n\n");
	else
		printf("stack isn't empty -- correct\n\n");
	array = LStackPop(stack);
	if (array != NULL)
		printf("vratilo se!\n\n");
	if (LStackEmpty(stack))
		printf("stack is empty -- correct\n\n");
	else
		printf("stack isn't empty -- wrong\n\n");
	
	free(stack);
	free(array);*/

/*int StackSize = 10;
	vararr array = malloc(sizeof(void*) * 2);
	array[0] = malloc(sizeof(int));
	array[1] = malloc(sizeof(double));

	*((int*) array[0]) = 1;
	*((double*) array[1]) = 2.0;

	tLVS *stack = malloc(sizeof(tLVS));
	stack->l_stack = malloc(sizeof(vararr)*StackSize);
	LStackInit(stack,StackSize);
	LStackPush(stack, array);

	array = NULL;
	array = LStackPop(stack);

	printf("%d --- %f\n", *((int*) array[0]), *((double*) array[1]));
	
	free(stack);
	free(array);*/



/*int *a = malloc(sizeof(int));
	*a = 5;
	
	int *b = malloc(sizeof(int));
	
	char *c = malloc(sizeof(char)*5);
	memcpy(c,"ahoj",5);

	char *d = malloc(sizeof(char)*5);

	double *e = malloc(sizeof(double));
	double *f = malloc(sizeof(double));

	*e = 9.0;

	VarStackPush(stack,a,NULL,NULL,NULL);

	*b = IntVarStackPop(stack);

	VarStackPush(stack, NULL, NULL, c,NULL);
	
	memcpy(d, StrVarStackPop(stack), 5);

	VarStackPush(stack, NULL, e, NULL,NULL);

	*f = DoubleVarStackPop(stack);*/


/*tVarS *stack = malloc(sizeof(tVarS));
	stack->var_stack = malloc(sizeof(void*)*StackSize);
	VarStackInit(stack, StackSize);
	
	void **a = malloc(sizeof(void*)*3);

	a[0] = malloc(sizeof(int));
	a[1] = malloc(sizeof(double));
	a[2] = malloc(sizeof(char) * 3);

	*((int*) a[0]) = 5;
	*((double*) a[1]) = 10.0;
	memcpy(((char*) a[2]), "ah", sizeof(char)*3);

	VarStackPush(stack, ((int*) a[0]), NULL, NULL, NULL);
	VarStackPush(stack, NULL, ((double*) a[1]), NULL, NULL);
	VarStackPush(stack, NULL, NULL, ((char*) a[2]), NULL);

	*((int*) a[0]) = 1;
	*((double*) a[1]) = 0.0;
	memcpy(((char*) a[2]), "ha", sizeof(char)*3);

	memcpy(((char*) a[2]), StrVarStackPop(stack), sizeof(char)*3);
	*((double*) a[1]) = DoubleVarStackPop(stack);
	*((int*) a[0]) = IntVarStackPop(stack);

	

	printf("%d -- %f -- %s\n\n",*((int*) a[0]),*((double*) a[1]),((char*) a[2]));*/



/*int StackSize = 10;
	tAddS *stack = malloc(sizeof(tAddS));
	stack->add_stack = malloc(sizeof(tItem)*StackSize);
	AddStackInit(stack, StackSize);

	tInstr I;
	I.Type = 0;
	I.addr1 = NULL;
	I.addr2 = NULL;
	I.addr3 = NULL;

	tItem *item = malloc(sizeof(tItem));
	item->Instruction = I;
	item->NextItem = NULL;

	AddStackPush(stack, item);
	
	item = NULL;

	if (item == NULL)
		printf("yep\n");
	else
		printf("nope\n");

	item = AddStackPop(stack);
	
	if (item != NULL)
		printf("yep\n");
	else
		printf("nope\n");*/
