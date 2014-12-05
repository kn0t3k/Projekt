/*--------------------------------------
| Projekt: IFJ14						
| Soubor: parser.h						
| Autori: Denk Filip (xdenkf00)		
|	  Jurica Tomas (xjuric22)		
|	  Knotek Martin (xknote11)	
|	  Kohut Jan (xkohut08)		
|	  Piskula David (xpisku02)	
| Prekladac: GCC 4.8.2-7				
| Datum:   5.12.2014					
|--------------------------------------*/

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
int type(struct htab_item *item);
int function();
int parameter(struct htab_item *func_item);
int n_parameter(struct htab_item *func_item,int* counter);
int function_body(struct htab_item *func_item);
int function_readln();
int function_write();
int callfunass();
int callorass(int expected_type_of_result);
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
int expression(int expected_type_of_result);
int assign_int_to_token(int token);
int table_symbols(int x, int y, PtrStack Stack);
int parse_expression(int expected_type_of_result);
int type_control(struct htab_item* operand_1, int operator, struct htab_item* operand_2);

/*Funkce pro zasobnik*/
void SInit(PtrStack Stack);
int SPush(PtrStack Stack, int data, struct htab_item * item);
int STopExpression(PtrStack Stack);
void SPop(PtrStack Stack);
int STop(PtrStack Stack);
void SEmpty(PtrStack Stack);

