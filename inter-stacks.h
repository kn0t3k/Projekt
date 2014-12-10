#include <stdbool.h>

typedef void** vararr; //variable array

typedef struct LVS{ //Local Variable Stack
	vararr *l_stack;
	int top;
	int StackSize;
} tLVS; 


typedef struct VarS{ //Parameter and Return Variable Stack
	vararr var_stack;
	int top;
	int StackSize;
} tVarS;

typedef struct AddS{ //Return Address Stack
	void **add_stack;
	int top;
	int StackSize;
} tAddS;


void LStackInit (tLVS *S, int StackSize);
int LStackEmpty (tLVS *S);
int LStackPush (tLVS *S, vararr Array);
vararr LStackPop (tLVS *S);
//-------------------
void VarStackInit (tVarS *S, int StackSize);
int VarStackEmpty (tVarS *S);
int VarStackPush (tVarS *S, int *intvar, double *doublevar, char *stringvar, bool *boolvar);
int IntVarStackPop (tVarS *S);
double DoubleVarStackPop (tVarS *S);
char* StrVarStackPop (tVarS *S);
bool BoolVarStackPop (tVarS *S);
void DisposeVarStack (tVarS *S);
//--------------------
void AddStackInit (tAddS *S, int StackSize);
int AddStackEmpty (tAddS *S);
int AddStackPush (tAddS *S, void *item);
void* AddStackPop (tAddS *S);
