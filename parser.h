#include "ial.h"

/*Struktury pro zasobnik*/
typedef struct Element{
  int data;
  struct htab_item * item;  
  struct Element * Next;
}*PtrElement;

typedef struct StructStack{
  PtrElement Top;
}*PtrStack;

/*Prototypy funkci*/

int parse();

/*Funkce, ktere predstavuji nonterminaly*/
int program();
int declaration();
int n_declaration();
int type(struct htab_item **item);
int function();
int parameter(struct htab_item **func_item);
int n_parameter(struct htab_item **func_item,int* counter);
int function_body(struct htab_item **func_item);
int function_readln();
int function_write();
int callfunass();
int callorass(struct htab_item **expected_item);
int id_function();
int value(struct htab_item** item);
int variable(struct htab_item **func_item);
int n_variable();
int while_condition();
int if_condition();
int body();
int element();
int n_element();
int select_element();
int expression(struct htab_item **expected_item);
int assign_int_to_token(int token);
int table_symbols(int x, int y, PtrStack Stack);
int parse_expression(struct htab_item **expected_item);
int type_control(struct htab_item** operand_1, int operator, struct htab_item** operand_2);

/*Funkce pro zasobnik*/
void SInit(PtrStack Stack);
int SPush(PtrStack Stack, int data, struct htab_item *item);
int STopExpression(PtrStack Stack);
void SPop(PtrStack Stack);
int STop(PtrStack Stack);
void SEmpty(PtrStack Stack);

