#include <stdbool.h>

typedef void** vararr;

/*typedef struct GVS{ //Global Variable Stack
	vararr *g_stack;
	int top;
} tGVS;*/

typedef struct LVS{ //Local Variable Stack
	vararr *l_stack;
	int top;
	int StackSize;
} tLVS; 

struct Element{
	//int type;
	void *variable;
};

typedef struct VarS{ //Parameter and Return Variable Stack
	struct Element *var_stack;
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
void LStackPush (tLVS *S, vararr Array);
vararr LStackPop (tLVS *S);
//-------------------
void VarStackInit (tVarS *S, int StackSize);
int VarStackEmpty (tVarS *S);
void VarStackPush (tVarS *S, int *intvar, double *doublevar, char *stringvar, bool *boolvar);
int IntVarStackPop (tVarS *S);
double DoubleVarStackPop (tVarS *S);
char* StrVarStackPop (tVarS *S);
bool BoolVarStackPop (tVarS *S);
//--------------------
void AddStackInit (tAddS *S, int StackSize);
int AddStackEmpty (tAddS *S);
void AddStackPush (tAddS *S, void *item);
void* AddStackPop (tAddS *S);
