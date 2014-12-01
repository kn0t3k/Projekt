#define SYNTAX_OK     -1
#define SYNTAX_ERROR -2
#define SEM_ERROR    -3

#define SHIFT 65
#define REDUCE 63
#define EQUAL 47
#define ERROR 89
#define DOLLAR 78

/*Struktury pro zasobnik*/
typedef struct Element{
  int data;  
  struct Element * Next;
}*PtrElement;

typedef struct Stack{
  PtrElement Top;
}*PtrStack;

/*Prototypy funkci*/

int parse();

/*Funkce, ktere predstavuji nonterminaly*/
int program();
int declaration();
int n_declaration();
int type();
int function();
int parameter();
int n_parameter();
int function_body();
int function_readln();
int function_write();
int callfunass();
int callorass();
int id_function();
int value();
int variable();
int n_variable();
int while_condition();
int if_condition();
int body();
int element();
int n_element();
int select_element();
int expression();
int assign_int_to_token(int token);
int table(int x, int y, PtrStack Stack);
int parse_expression();

/*Funkce pro zasobnik*/
void SInit(PtrStack Stack);
void SPush(PtrStack Stack, int data);
int STopExpression(PtrStack Stack);
void SPop(PtrStack Stack);
int STop(PtrStack Stack);
void SEmpty(PtrStack Stack);

