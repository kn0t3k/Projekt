#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <errno.h> 
#include "err.h"
#include "str.h"
#include "ial.h"
#include "scaner.h"
#include "list.h"
#include "parser.h"
#include "interpret.h"



void vypis(int vysledek){

switch (vysledek)
        {
            case ID: printf("ID");break;
            case SHIFT:printf("SHIFT");break;
            case REDUCE:printf("REDUCE");break;
            case EQUAL:printf("EQUAL");break;
            case DOLLAR:printf("DOLLAR");break;
            case ID_FUNCTION:printf("ID_FUNCTION");break;
            case BEGIN:printf("BEGIN");break;
            case BOOLEAN:printf("BOOLEAN");break;
            case DO:printf("DO");break;
            case ELSE:printf("ELSE");break;
            case END:printf("END");break;
            case FALSE:printf("FALSE");break;
            case FIND:printf("FIND");break;
            case FORWARD:printf("FORWARD");break;
            case FUNCTION:printf("FUNCTION");break;
            case IF:printf("IF");break;
            case T_INTEGER:printf("T_INTEGER");break;
            case READLN:printf("READLN");break;
            case T_REAL:printf("T_REAL");break;
            case SORT:printf("SORT");break;
            case T_STRING:printf("T_STRING");break;
            case THEN:printf("THEN");break;
            case TRUE:printf("TRUE");break;
            case VAR:printf("VAR");break;
            case WHILE:printf("WHLE");break;
            case WRITE:printf("WRITE");break;
            case STRING:printf("STRING");break;
            case INTEGER:printf("INTEGER");break;
            case REAL:printf("REAL");break;
            case COPY:printf("COPY");break;
            case LENGTH:printf("LENGTH");break;
            case EXPRESSION:printf("EXPRESSION");break;
            case ADD:printf("ADD");break;
            case DIF:printf("DIF");break;
            case MUL:printf("MUL");break;
            case DIV:printf("DIV");break;
            case EQ:printf("EQ");break;
            case L:printf("L");break;
            case S:printf("S");break;
            case LE:printf("LE");break;
            case SE:printf("SE");break;
            case SL:printf("SL");break;
            case ASS:printf("ASS");break;
            case DES_INT:printf("DES_INT");break;
            case DES_EXP:printf("DES_EXP");break;
            case DES_EXP_NEG:printf("DES_EXP_NEG");break;
            case EXP:printf("EXP");break;
            case EXP_NEG:printf("EXP_NEG");break;
            case LEFT_VINCULUM:printf("LEFT_VINCULUM");break;
            case RIGHT_VINCULUM:printf("RIGHT_VINCULUM");break;
            case SEMICOLON:printf("SEMICOLON");break;
            case APS:printf("APS");break;
            case L_BRACKET:printf("L_BRAKET");break;
            case R_BRACKET:printf("R_BRACKET");break;
            case COLON:printf("COLON");break;
            case COMMA:printf("COMA");break;
            case DOT:printf("DOT");break;
        }
}


/*Globalni promenne*/
int token;
int counterVar = 1;
struct symbol_table* table;
PtrStack Stack;
string attr;
string *str_parameters = NULL;

tList *LS;

void generateVariable(string *var)

{
  strClear(var);
  strAddChar(var, '$');
  int i;
  i = counterVar;
  while (i != 0)
  {
    strAddChar(var, (char)(i % 10 + '0'));
    i = i / 10;
  }
  counterVar ++;
} 

int generateInstruction(int instType, void *addr1, void *addr2, void *addr3)

{
   int check = 0;
   tInstr I;
   I.Type = instType;
   I.addr1 = addr1;
   I.addr2 = addr2;
   I.addr3 = addr3;
   check = InsertNew(LS, I);
   return check;
}
 


int parse(struct symbol_table* table_main, tList *list, PtrStack Stack_main){
  
  int result;
  table = table_main;
  Stack = Stack_main;
  strInit(&attr);
  
  LS = list;
  
  if ((token = getNextToken(&attr)) == LEX_ERROR)
     result = LEX_ERROR;
  else
     result = program();
  strFree(&attr);
  
  if (str_parameters != NULL){
    strFree(str_parameters);
    free(str_parameters);
    }	
  
  return result;
}

int program(){ /*<PROGRAM>*/
  
  
  int result;
  void *label;
  
  
  switch (token){
    /*<PROGRAM> -> <DECLARATION> <FUNCTION> <BODY> DOT*/
    case VAR:
    case FUNCTION:
    case BEGIN:
      result = declaration();
	  if (result != SYNTAX_OK) return result;/*<DECLARATION>*/
      result = function();	  
	  if (result != SYNTAX_OK) return result;/*<FUNCTION>*/
	  if (funcs_defined(table) == SEM_ERROR) return SEM_ERROR;
	  remove_local_table(table, &result);
	  if ((generateInstruction(I_LABEL, NULL, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  label =  LastItemAddress(LS);
	  SetFirst(LS, label);
	  result = body();	  
	  if (result != SYNTAX_OK) return result;/*BODY*/  
	  if (token != DOT) return SYNTAX_ERROR;/*DOT*/
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != END_OF_FILE) return SYNTAX_ERROR;
	  if ((generateInstruction(I_END, NULL, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  return SYNTAX_OK;/*Pokud zadne z pravidel nevrati SYNTAX_ERROR, LEX_ERROR nebo SEM_ERROR*/
	  break;
	
	default:/*Pokud je prvni token jiny nez povoleny*/
      return SYNTAX_ERROR;	
	  break;
	}	
}

int declaration(){/*<DECLARATION>*/

  int result, error;
  struct htab_item *item;
  

  switch (token){
    /*<DECLARATION> -> VAR ID COLON <TYPE> SEMICOLON <N_DECLARATION>*/
	case VAR:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != ID) return SYNTAX_ERROR;  
	  if ((item = add_var(attr.str, table, &error)) == NULL)
        return error;	  
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != COLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = type(&item);
      if (result != SYNTAX_OK) return result;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = n_declaration();
	  if (result != SYNTAX_OK) return result;
      return SYNTAX_OK;	      
	  break;
	
	/*<DECLARATION> -> eps*/
	case FUNCTION:
	case BEGIN:
	  return SYNTAX_OK;
	  break;
	  
	default:/*Nemuze nastat, ale z konvence to pridavam*/
	  return SYNTAX_ERROR;
	  break;
  }
}

int type(struct htab_item **item){/*<TYPE>*/

  switch (token){
    /*<TYPE> -> T_INTEGER, pro ostatni case analogicky*/  
    case T_INTEGER:
      if (str_parameters != NULL){
        strAddChar(str_parameters, 'i');
		}
	  if ((*item) -> type == s_default)/*Jestlize jeste nebyl pridan typ promene, tak chceme priradit, jinak se provadi pouze kontrola*/
	    (*item) -> type = s_integer;	  
	  break;
	  
	case T_REAL:
	  if (str_parameters != NULL)
        strAddChar(str_parameters, 'r');
	  if ((*item) -> type == s_default)
	    (*item) -> type = s_real;		
	  break;
	  
	case T_STRING:
      if (str_parameters != NULL)
        strAddChar(str_parameters, 's');
	  if ((*item) -> type == s_default)
	    (*item) -> type = s_string;		
	  break;
	  
	case T_BOOLEAN:
      if (str_parameters != NULL)
        strAddChar(str_parameters, 'b');
	  if ((*item) -> type == s_default)
	    (*item) -> type = s_boolean;	  
	  break;
	  
	default:
      return SYNTAX_ERROR;
	  break;
  }
  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
  return SYNTAX_OK;  
}

int n_declaration(){/*<N_DECLARATION>*/
  
  int result, error;
  struct htab_item *item;
  
  switch (token){
    /*<N_DECLARATION> -> ID COLON <TYPE> SEMICOLON <N_DECLARATION>*/
    case ID:
	  if ((item = add_var(attr.str, table, &error)) == NULL)
        return error;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != COLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = type(&item);
	  if (result != SYNTAX_OK) return result;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = n_declaration();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;

	/*<N_DECLARATION> -> eps*/
	case FUNCTION:
	case BEGIN:
	  return SYNTAX_OK;
	  break;
	
    default:
      return SYNTAX_ERROR;
	  break;
  }	  
}

int function(){/*<FUNCTION>*/

  int result;
  struct htab_item *func_item;
  struct htab_item *item;
  
  switch (token){
    /*<FUNCTION> -> FUNCTION ID_FUNCTION L_BRACKET <PARAMETER> R_BRACKET COLON <TYPE> SEMICOLON <FUNCTION_BODY> <FUNCTION>*/
    case FUNCTION:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != ID_FUNCTION) return SYNTAX_ERROR;
	  if ((func_item = search_func(attr.str, table, &result)) != NULL){
		if (func_item -> initialized == 1)
		  return SEM_ERROR;
		table -> local = func_item -> func_table;/*Pokud uz je funkce deklarovana, prejdeme do jejiho lokalniho ramce*/
        }
      else{
  	    add_local_table(table, &result);
	    if ((func_item = add_func(attr.str, table, &result)) == NULL)
	      return result;
		  }
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != L_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = parameter(&func_item);
	  if (result != SYNTAX_OK) return result;
	  if (token != R_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != COLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  
	  if ((item = search_var(func_item -> name, table, &result)) == NULL)
	    return result;
		
	  if (func_item -> fwd == 1){/*Overime, jestli ma funkce stejny typ jako jeji dopredna deklarace*/
	    switch(token){
		  case T_INTEGER:
		    if (item -> type != s_integer)
			  return SEM_ERROR;
			break;
          
		  case T_REAL:
 		    if (item -> type != s_real)
			  return SEM_ERROR;
			break;
		
		  case T_STRING:
		    if (item -> type != s_string)
			  return SEM_ERROR;
			break;
			
		  case T_BOOLEAN:
		    if (item -> type != s_boolean)
			  return SEM_ERROR;
			break;
		  }
		if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
		result = SYNTAX_OK;  
		}
	  else{
	    result = type(&item);/*Nastavime navratovy typ funkce do specialni lokalni polozky pro funkci*/
		func_item -> type = item -> type;/*Nastavime navratovy typ do polozky pro danou funkci v globalni tabulce*/
		}
		
	  if (result != SYNTAX_OK) return result;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = function_body(&func_item);
	  if (result != SYNTAX_OK) return result;
	  result = function();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	
    /*<FUNCTION> -> eps*/	
	case BEGIN:
	  return SYNTAX_OK;
	  break;
	
	default:
	  return SYNTAX_ERROR;
	  break;
	}
}

int parameter(struct htab_item **func_item){/*<PARAMETER>*/

  int result;
  int counter = 0;/*Citac parametru*/
  int porovnani = 0;/*Predstavuje boolean, pokud uz je funkce deklarovana, pouze srovnamvame parametry u definice*/ 
  struct htab_item *item;

  if((str_parameters = (string *) malloc(sizeof(*str_parameters))) == NULL)
    return INTERNAL_ERR;
  strInit(str_parameters);
  
  switch (token){
    /*<PARAMETER> -> ID COLON <TYPE> <N_PARAMETER>*/
	case ID:
	  counter = 1;	  	
	  if ((*func_item) -> fwd == 1){
	    porovnani = 1;
	    if ((item = search_var(attr.str, table, &result)) != NULL){
          if (item -> index != counter)
		    return SEM_ERROR;
			}
		else
		  return SEM_ERROR;
	    }
      else{
        if ((item = add_var(attr.str, table, &result)) == NULL)
          return result;
		else
		  item -> initialized = 1;
        }	  	
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != COLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = type(&item);
	  if (result != SYNTAX_OK) return result;
	  result = n_parameter(func_item, &counter);
	  if (result != SYNTAX_OK) return result;
	  break;
	
	/*<PARAMETER> -> eps*/
	case R_BRACKET:
	  break;
	  
	default: 
	  return SYNTAX_ERROR;
	  break;
    }
  
  /*Pokud jsme delali porovnani srovname, jestli se retezce parametru shoduji*/
  
  if (porovnani){
    printf ("\nPorovnavam parametry funkci: forward = %s, aktualni = %s",(*func_item) -> func_data, str_parameters -> str);
    if (strcmp((*func_item) -> func_data, str_parameters -> str) != 0)
        return SEM_ERROR;		
    }
  else{/*Jinak pridavame parametry, musime do polozky pro funkci v globalni tabulce zapsat retezec obsahujici parametry*/
    if (counter != 0){
	  (*func_item) -> func_data = (char*) malloc(sizeof(char)*(strlen(str_parameters -> str)+1));
	  if((*func_item) -> func_data == NULL){
		return  INTERNAL_ERR;;
	    }
	  printf("\nPridavam parametry k funkci (%s) : %s", (*func_item) -> name, str_parameters -> str);
	  strncpy((*func_item) -> func_data, str_parameters -> str, sizeof(char)*(strlen(str_parameters -> str)+1));
	  }
	}
  strFree(str_parameters);	
  free(str_parameters);
  str_parameters = NULL;  
  
  return SYNTAX_OK;
}

int n_parameter(struct htab_item **func_item,int* counter){/*<N_PARAMETER>*/

  int result;
  struct htab_item *item;
  
  switch (token){
    /*<N_PARAMETER> -> SEMICOLON ID COLON <TYPE> <N_PARAMETER>*/
	case SEMICOLON:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != ID) return SYNTAX_ERROR;
	  if ((*func_item) -> fwd == 1){
	    if ((item = search_var(attr.str, table, &result)) != NULL){
		  (*counter)++;
          if (item -> index != *counter)
		    return SEM_ERROR;
			}
		else
		  return SEM_ERROR;
	    }
      else{
        if ((item = add_var(attr.str, table, &result)) == NULL)
          return result;
		else
          item -> initialized = 1;		
        }	  	
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != COLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = type(&item);
	  if (result != SYNTAX_OK) return result;
	  result = n_parameter(func_item, counter);
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	
    /*<N_PARAMETER> -> eps*/	
	case R_BRACKET:
	  return SYNTAX_OK;
	  break;
	  
	default:
	  return SYNTAX_ERROR;
	  break;
    }
}

int function_body(struct htab_item **func_item){/*<FUNCTION_BODY>*/

  int result;
  struct htab_item* item = NULL;
  
  switch (token){
    /*<FUNCTION_BODY> -> FORWARD SEMICOLON*/
    case FORWARD:
	  if ((*func_item) -> fwd == 1)
        return SEM_ERROR;
      else
        (*func_item) -> fwd = 1;	  
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != SEMICOLON) return SYNTAX_ERROR;	
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  return SYNTAX_OK;
	  break;
	
	/*<FUNCTION_BODY> -> <DECLARATION> <BODY> SEMICOLON*/
	case VAR:
	case BEGIN:
	  if ((*func_item) -> initialized == 1)
	    return SEM_ERROR;
	  else
	    (*func_item) -> initialized = 1;
	  result = declaration();
	  if (result != SYNTAX_OK) return result;
	  if ((generateInstruction(I_LABEL, NULL, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  (*func_item) -> label = LastItemAddress(LS);
	  result = body();
	  if (result != SYNTAX_OK) return result;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  if ((item = search_var((*func_item) -> name, table, &result)) == NULL)
		return result;
      if (item -> initialized != 1){/*Overime, ze funkce neco vraci*/
		printf("\nFunkce (%s) by mela vracet nejaky typ",(*func_item) -> name);
		return SEM_ERROR;
		}
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if ((generateInstruction(I_RETURN, NULL, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  return SYNTAX_OK;
	  break;
	  
	default:
	  return SYNTAX_ERROR;
	  break;
    }
}

int body(){/*<BODY>*/
  
  int result;
  
  switch (token){
    /*<BODY> -> BEGIN <ELEMENT> END*/
    case BEGIN:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = element();
	  if (result != SYNTAX_OK) return result;
	  if (token != END) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  return SYNTAX_OK;
	  break;
	
	default:
	  return SYNTAX_ERROR;
	  break;
  }
}

int element(){/*<ELEMENT>*/
 
  int result;
 
  switch (token){
    /*<ELEMENT> -> <SELECT_ELEMENT> <N_ELEMENT>*/
    case BEGIN:
	case ID:
	case WHILE:
	case IF:
	case WRITE:
	case READLN:
	  result = select_element();
	  if (result != SYNTAX_OK) return result;
	  result = n_element();
      if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	
	/*<ELEMENT> -> eps*/
    case END:
       return SYNTAX_OK;
       break;

    default:
      return SYNTAX_ERROR;
      break;	  
    } 
}

int select_element(){/*<SELECT_ELEMENT>*/

  int result;
  
  switch (token){
    /*<SELECT_ELEMENT> -> <BODY>*/
	case BEGIN:
	  result = body();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	  
	 /*<SELECT_ELEMENT> -> <CALLFUNASS>*/  
	case ID:
	  result = callfunass();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	  
	 /*<SELECT_ELEMENT> -> <IF>*/  
	case WHILE:
	  result = while_condition();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	
	 /*<SELECT_ELEMENT> -> <WHILE>*/
	case IF:
	  result = if_condition();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
    
	/*<SELECT_ELEMENT> -> <FUNCTION_READLN>*/
	case READLN:
	  result = function_readln();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	
    /*<SELECT_ELEMENT> -> <FUNCTION_WRITE>*/	
	case WRITE:
	  result = function_write();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	
    default:
      return SYNTAX_ERROR;
      break;
    }	  
}

int n_element(){/*<N_ELEMENT>*/

  int result;
 
  switch (token){
    /*<N_ELEMENT> -> SEMICOLON <SELECT_ELEMENT> <N_ELEMENT>*/
    case SEMICOLON:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = select_element();
	  if (result != SYNTAX_OK) return result;
	  result = n_element();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	
	/*<N_ELEMENT> -> eps*/
	case END:
	  return SYNTAX_OK;
	  break;
	
	default:
	  return SYNTAX_ERROR;
	  break;
    }
}

int callfunass(){/*<CALLFUNASS>*/
  
  int result;
  int is_call_of_function = 0; /*Pokud se bude volat funkce, hodnota se do id nakopiruje uz pri instrukci I_CALL, potrebuje preskocit instrukci I_COPYVAR*/
  struct htab_item *expression_item;/*Ukazatel, ktery posleme dal, bude v nem ukazatel na zpracovany vyraz nebo ukazatel na to co vratila funkce*/
  struct htab_item *id_item;/*Ukazatel v kterem si zapamatujeme ID do ktereho prirazujeme*/
  
  switch (token){
    /*<CALLFUNASS> -> ID ASS <CALLORASS>*/
    case ID:
	  if ((id_item = search_var(attr.str, table, &result)) == NULL)
        return result;
	  if (id_item -> function == 1)/*Overime, ze neprirazujeme do funkce*/
        return SEM_ERROR;	
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != ASS) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  expression_item = id_item;
	  result = callorass(&expression_item, &is_call_of_function);
	  if (result != SYNTAX_OK) return result;
	  id_item -> initialized = 1;
	  if (is_call_of_function == 0)
	    if ((generateInstruction(I_COPYVAR, (void *) expression_item, NULL, (void *) id_item)) == INTERNAL_ERR) return INTERNAL_ERR;   
	  return SYNTAX_OK;
	  break;
    
    default:
      return SYNTAX_ERROR;
      break;	  
    }
}

int callorass(struct htab_item **expected_item, int *is_call_of_function){

  int result;
  struct htab_item *func_item;
  
  switch (token){
    /*<CALLORASS> -> <EXPRESSION>*/
    case ID:
	case INTEGER:
	case STRING:
	case BOOLEAN:
	case DES_INT:
	case DES_EXP:
	case DES_EXP_NEG:
	case EXP:
	case EXP_NEG:
	case L_BRACKET:
	  result = expression(expected_item);
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	 
    /*<CALLORASS> -> ID_FUNCTION L_BRACKET <VARIABLE> R_BRACKET*/  
	case ID_FUNCTION:
	  *is_call_of_function = 1;/*Jedna se o volani funkce*/
	  if ((func_item = search_func(attr.str, table, &result)) == NULL)
	    return result;
	  else{
	    if ((func_item -> fwd != 1) && (func_item -> initialized != 1))/*Funkce musi byt minimalne deklarovana*/
		  return SEM_ERROR;
		if (func_item -> type != (*expected_item) -> type)/*Overime, jestli se navratovy typ funkce shoduje s typem promenne do ktere prirazujeme*/
		  return SEM_ERROR;
	    }   
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != L_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = variable(&func_item);
	  if (result != SYNTAX_OK) return result;
	  if (token != R_BRACKET) return SYNTAX_ERROR;
	 
	  if (strcmp(func_item -> name, "LENGTH") == 0){
	     if ((generateInstruction(I_LENGTH, (void *) func_item, NULL, (void *) (*expected_item))) == INTERNAL_ERR) return INTERNAL_ERR; //instrukce pro LENGTH
      }
	  else{
	    if (strcmp(func_item -> name, "COPY") == 0){
		   if ((generateInstruction(I_COPY, (void *) func_item, NULL, (void *) (*expected_item))) == INTERNAL_ERR) return INTERNAL_ERR; //instrukce pro COPY
        }
		else{
		  if (strcmp(func_item -> name, "FIND") == 0){
		    if ((generateInstruction(I_FIND, (void *) func_item, NULL, (void *) (*expected_item))) == INTERNAL_ERR) return INTERNAL_ERR; //instrukce pro FIND
          }
		  else{
		    if (strcmp(func_item -> name, "SORT") == 0){
			   if ((generateInstruction(I_SORT, (void *) func_item, NULL, (void *) (*expected_item))) == INTERNAL_ERR) return INTERNAL_ERR; //instrukce pro SORT
            }
			else{
               if ((generateInstruction(I_CALL, (void *) func_item, NULL, (void *) (*expected_item))) == INTERNAL_ERR) return INTERNAL_ERR;//instrukce pro CALL
            }
            }
		  }
        }		
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  return SYNTAX_OK;
	  break;
	  
	default:
	  return SYNTAX_ERROR;
	  break;
	}  
}


int variable(struct htab_item **func_item){

  int result;
  struct htab_item *item = NULL;
  
  if((str_parameters = (string *) malloc(sizeof(*str_parameters))) == NULL)
    return INTERNAL_ERR;
  strInit(str_parameters);
  
  switch (token){
    /*<VARIABLE> -> <VALUE> <N_VARIABLE>*/
	case ID:
	case INTEGER:
	case STRING:
	case BOOLEAN:
	case DES_INT:
	case DES_EXP:
	case DES_EXP_NEG:
	case EXP:
	case EXP_NEG:
	  result = value(&item);
	  if (result != SYNTAX_OK) return result;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = n_variable();
	  if (result != SYNTAX_OK) return result;
	  break;
	
	/*<VARIABLE> -> eps*/
	case R_BRACKET:
	  break;
	  
	default:
	   return SYNTAX_ERROR;
	   break;  
	} 

  if (func_item != NULL){
    printf("\nPorovnavam parametry volane funkce (%s): %s se zadanymi parametry: %s", (*func_item) -> name, (*func_item) -> func_data, str_parameters -> str);
    if ((*func_item) -> func_data == NULL){
	  if (strcmp("", str_parameters -> str) != 0)
	    return SEM_ERROR_TYPE;
	  }	
    else{ 
	  if (strcmp((*func_item) -> func_data, str_parameters -> str) != 0)
        return SEM_ERROR_TYPE;
	  }
	}
   else{
     char *write_string;
     if ((write_string = (char*) malloc(sizeof(char)*(strlen(str_parameters -> str)+1))) == NULL)/*Alokace mista pro hodnotu promenne*/
	    return INTERNAL_ERR;
	 strncpy(write_string, str_parameters -> str, sizeof(char)*(strlen(str_parameters -> str)+1));
	 if ((generateInstruction(I_WRITE, NULL, NULL, (void *) write_string)) == INTERNAL_ERR) return INTERNAL_ERR;
     }
  
  strFree(str_parameters);	
  free(str_parameters);
  str_parameters = NULL;
  return SYNTAX_OK;  
}

int value(struct htab_item** item){
  
  int result;
  string new_variable;
  
  switch (token){
    case ID:/*Pokud jde o ID vyhleda se v aktualni tabulce, jestli je tam a pak se zkontroluje inicializace*/
	  if (((*item) = search_var(attr.str, table, &result)) == NULL)
        return result;
	  else{/*Zkontrolujeme, zda muzeme ID pouzit*/
	    if (table -> local != table -> global){
	      if ((*item) -> global != 1){/*Globalni promenne, nevyzaduji inicializaci*/
			if (((*item) -> initialized != 1) || ((*item) -> function == 1)){/*Kontrolujeme, zda se nejedna o funkci*/
			  return SEM_ERROR;
			  }
	        }
		  else{
		    if ((*item) -> function == 1)
		      return SEM_ERROR;
		    }
	      }
		else{/*Pokud jsme jiz v globalnim tele programu tak inicializaci globalni promenne vyzadujeme*/
		  if (((*item) -> initialized != 1) || ((*item) -> function == 1))
	         return SEM_ERROR;
		  }
		}
      break;
			
	case INTEGER:/*Pokud je to int, tak se vytvori novy jedinecny nazev, a pridame do tabulky polozku s timto nazvem + data + typ*/
	  ;
      int *value_i;
	  if ((value_i = (int *) malloc(sizeof(int))) == NULL)/*Alokace mista pro hodnotu promenne*/
	    return INTERNAL_ERR;
	  *value_i = atoi(attr.str);
	  strInit(&new_variable);
	  generateVariable(&new_variable);
	  if (((*item) = add_var(new_variable.str, table, &result)) == NULL){
	    strFree(&new_variable);
	    free(value_i);
	    return result;
	    }
	  strFree(&new_variable);	
	  (*item) -> type = s_integer;
	  if ((generateInstruction(I_ASSIGN,(void *) value_i, NULL, (void *) (*item))) == INTERNAL_ERR) return INTERNAL_ERR;
	  break;
			
    case STRING:
      ;
	  char *value_s;
	  if ((value_s = (char*) malloc(sizeof(char)*(strlen(attr.str)+1))) == NULL)/*Alokace mista pro hodnotu promenne*/
	    return INTERNAL_ERR;
	  strncpy(value_s, attr.str, sizeof(char)*(strlen(attr.str)+1));  
	  strInit(&new_variable);
	  generateVariable(&new_variable);
	  if (((*item) = add_var(new_variable.str, table, &result)) == NULL){
	    strFree(&new_variable);
	    free(value_s);
	    return result;
	    }
	  strFree(&new_variable);
      (*item) -> type = s_string;
      if ((generateInstruction(I_ASSIGN,(void *) value_s, NULL, (void *) (*item))) == INTERNAL_ERR) return INTERNAL_ERR;		
	  break;
			
	case BOOLEAN:
	  ;
	  bool *value_b;
      if ((value_b = (bool *) malloc(sizeof(bool))) == NULL)/*Alokace mista pro hodnotu promenne*/
	    return INTERNAL_ERR;
	  if (strcasecmp(attr.str, "true") == 0)
	    *value_b = 1;
	  else
	    *value_b = 0;
	  strInit(&new_variable);
	  generateVariable(&new_variable);
	  if (((*item) = add_var(new_variable.str, table, &result)) == NULL){
	    strFree(&new_variable);
	    free(value_b);
		return result;
	    }
	  strFree(&new_variable);	
	  (*item) -> type = s_boolean;
	  if ((generateInstruction(I_ASSIGN,(void *) value_b, NULL, (void *) (*item))) == INTERNAL_ERR) return INTERNAL_ERR;
	  break;
			
	  case DES_INT:
	  case DES_EXP:
	  case DES_EXP_NEG:
	  case EXP:
	  case EXP_NEG:
	    ;
	    double *value_d;
		if ((value_d = (double *) malloc(sizeof(double))) == NULL)/*Alokace mista pro hodnotu promenne*/
	      return INTERNAL_ERR;
		*value_d = strtod(attr.str,NULL);
		if (errno != 0){
		  free(value_d);
		  return SEM_ERROR_OVERFLOW;
		  }
		strInit(&new_variable);
	    generateVariable(&new_variable);
		if (((*item) = add_var(new_variable.str, table, &result)) == NULL){
		  strFree(&new_variable);
		  free(value_d);
		  return result;
		  }
		strFree(&new_variable);	
		(*item) -> type = s_real;
		if ((generateInstruction(I_ASSIGN,(void *) value_d, NULL, (void *) (*item))) == INTERNAL_ERR) return INTERNAL_ERR;
		break;
	  
	  default:
        (*item) = NULL;/*Nejedna se o hodnotu*/
		return SYNTAX_ERROR;
	    break;
	}
  if (str_parameters != NULL){/*Pokud porovnamvame parametry, potrebujeme tvorit retezec a pushnou parametr*/ 
    //Dopsat instrukci pro pushnuti parametru
	if ((generateInstruction(I_PUSH_PARAM, NULL, NULL,(void *) (*item))) == INTERNAL_ERR) return INTERNAL_ERR;
	switch ((*item) -> type){
      case 0:
	    strAddChar(str_parameters, 'i');
	    break;
	  case 1:
	    strAddChar(str_parameters, 'r');
	    break;
	  case 2:
	    strAddChar(str_parameters, 's');
	    break;
	  case 3:
	    strAddChar(str_parameters, 'b');
	    break;
	
	default:
	  break;
      }
	}
  return SYNTAX_OK;	
	  
}

int n_variable(){/*<N_VARIABLE>*/
  
  int result;
  struct htab_item *item = NULL;
  
  switch (token){
    /*<N_VARIABLE> -> COMMA <VALUE> <N_VARIABLE>*/
    case COMMA:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = value(&item);
	  if (result != SYNTAX_OK) return result;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = n_variable();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
    
	/*<N_VARIABLE> -> eps*/
    case R_BRACKET:
      return SYNTAX_OK;
      break;

    default:
      return SYNTAX_ERROR;	
	  break;
    }
}

int while_condition(){/*<WHILE_CONDITION>*/

  int result;
  struct htab_item* item = NULL;
  void *label1;
  void *label2;
  void *ifgotoadd;
  tInstr *Instruction;
  
  
  switch (token){
    /*<WHILE_CONDITION> -> WHILE EXPRESSION DO <BODY>*/
    case WHILE:
	  if ((generateInstruction(I_LABEL, NULL, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  label1 = LastItemAddress(LS);
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = expression(&item);
	  if (result != SYNTAX_OK) return result;
	  if (token != DO) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if ((generateInstruction(I_IFGOTO, (void *) item, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  ifgotoadd = LastItemAddress(LS);
	  result = body();
	  if (result != SYNTAX_OK) return result;
	  if ((generateInstruction(I_GOTO, NULL, NULL, label1)) == INTERNAL_ERR) return INTERNAL_ERR;
	  if ((generateInstruction(I_LABEL, NULL, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  label2 = LastItemAddress(LS);
	  GoToItem(LS, ifgotoadd);
	  Instruction = GetData(LS);
	  Instruction -> addr3 = label2;
	  return SYNTAX_OK;
	  break;
	  
	default:
	  return SYNTAX_ERROR;
	  break;
    }
}

int if_condition(){/*<IF_CONDITION>*/

  int result;
  struct htab_item* item = NULL;
  void *ifgotoadd;
  void *lastofif;
  void *elseadd;
  void *skipelse;
  tInstr *Instruction;
  
  switch (token){
    /*<IF_CONDITION> -> IF EXPRESSION THEN <BODY> ELSE <BODY>*/
	case IF:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = expression(&item);
	  if (result != SYNTAX_OK) return result;
	  if ((generateInstruction(I_IFGOTO, (void *) item, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  ifgotoadd = LastItemAddress(LS);
	  if (token != THEN) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = body();
	  if (result != SYNTAX_OK) return result;
	  if (token != ELSE) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if ((generateInstruction(I_GOTO, NULL, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  lastofif = LastItemAddress(LS);
	  if ((generateInstruction(I_LABEL, NULL, NULL, NULL)) == INTERNAL_ERR) return INTERNAL_ERR;
	  elseadd = LastItemAddress(LS);
	  GoToItem(LS, ifgotoadd);
	  Instruction = GetData(LS);
	  Instruction -> addr3 = elseadd;
	  result = body();
	  if (result != SYNTAX_OK) return result;
	  skipelse = LastItemAddress(LS);
	  GoToItem(LS, lastofif);
	  Instruction = GetData(LS);
	  Instruction -> addr3 = skipelse;
	  return SYNTAX_OK;
	  break;
	
	default:
	  return SYNTAX_ERROR;
	  break;
	}
}

int function_readln(){/*<FUNCTION_READLN>*/

  int result;
  struct htab_item* item;

  switch (token){
    /*<FUNCTION_READLN> -> READLN L_BRACKET ID R_BRACKET*/
	case READLN:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != L_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != ID) return SYNTAX_ERROR;
	  if ((item = search_var(attr.str, table, &result)) == NULL)
        return result;
	  if (item -> function == 1)
	    return SEM_ERROR;
	  else
	    item -> initialized = 1;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != R_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  //Volat instrukci pro READLN, item je ukazatel na danou polozku v tabulce, tu co je v zavorce
      if ((generateInstruction(I_READ, NULL, NULL, (void*) item)) == INTERNAL_ERR) return INTERNAL_ERR;
	  return SYNTAX_OK;
	  break;
	
	default:
	  return SYNTAX_ERROR;
	  break;
	  }
}

int function_write(){/*<FUNCTION_WRITE>*/

  int result;
  
  switch (token){
  /*<FUNCTION_WRITE> -> WRITE L_BRACKET <VARIABLE> R_BRACKET*/
    case WRITE:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != L_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = variable(NULL);
	  if (result != SYNTAX_OK) return result;
	  if (token != R_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  return SYNTAX_OK;
	  break;
		
	default:
	  return SYNTAX_ERROR;
	  break;
    }
}

int expression(struct htab_item **expected_item){/*<EXPRESSION>*/

  int result;

  switch (token){
    case ID:
	case INTEGER:
	case STRING:
	case BOOLEAN:
	case DES_INT:
	case DES_EXP:
	case DES_EXP_NEG:
	case EXP:
	case EXP_NEG:
	case L_BRACKET:
      result = parse_expression(expected_item);
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
      break;

    default:
      return SYNTAX_ERROR;
      break;	  
    }
}


int assign_int_to_token(int token){

  switch (token){
    case MUL:
	  return 100;
	  break;
	case DIV:
	  return 101;
	  break;
	case ADD:
      return 102;
	  break;
    case DIF:
      return 103;
      break;
    case S:
      return 104;
      break;
    case L:
      return 105;
      break;
    case SE:
      return 106;
      break;
    case LE:
      return 107;
      break;
    case EQ:
      return 108;
      break;
    case SL:
      return 109;
      break;
    case L_BRACKET:
      return 110;
      break;
    case R_BRACKET:
      return 111;
      break;
    case ID:
	case INTEGER:
	case STRING:
    case BOOLEAN:
	case DES_INT:
	case DES_EXP:
	case DES_EXP_NEG:
	case EXP:
	case EXP_NEG:
	  return 112;
	  break;
	case THEN:
	case DO:
	case SEMICOLON:
	case END:
	case DOLLAR:
	  return 113;
	  break;
	default:
	  printf("\nSpatny token u prirazovani int tokenum: %d",token);
	  return SYNTAX_ERROR;
	  break;
    }
  
 
}


int table_symbols(int x, int y, PtrStack Stack){/*Realizace tabulky*/
/*x je token na vrcholu zasobniku*/
/*y je vstupni token*/
  
  printf("\nNa vrcholu zasobniku je token: ");
  vypis(x);
  printf("\nAktualni token na vstupu je: "); 
  vypis(y);

  if ((x = assign_int_to_token(x)) == SYNTAX_ERROR) return SYNTAX_ERROR;  
  if ((y = assign_int_to_token(y)) == SYNTAX_ERROR) return SYNTAX_ERROR;
  
  
  if ((y <= 109) || (y == 111)){/*Vstupni token je operator, nebo prava zavorka, musime overit, ze na vrcholu zasobniku je vyraz*/
    if (((STopExpression(Stack)) == NEPRAVDA) && (x != 112) && (x != 111)){
	  return SYNTAX_ERROR;
	  }
    }
	
  if (x <= 109){/*Vstupni token je DOLLAR, musime overit, ze na vrcholu zasobniku neni operator*/
    if ((y == 113) && ((STopExpression(Stack)) == NEPRAVDA))
	  return SYNTAX_ERROR;
    }
  

  if ((y == 110) || (y == 112)){/*Vstupni token je leva zavorka, nebo hodnota, musime overit, ze na vrcholu zasobniku neni vyraz*/
	if ((STopExpression(Stack)) == PRAVDA){
	  return SYNTAX_ERROR;
	  }
    }
	

  printf("\nPodle tabulky rozhodnu co mam delat:");
  if (((x <= 101) && (y <= 109))||
      ((x > 101) && (x <= 103) && (y > 101) && (y <= 109))||
      ((x > 103) && (x <= 109) && (y > 103) && (y <= 109))||
      ((x > 110) && (x <= 112) && (y <= 109))||
      ((x <= 109) && (y == 111))||
      ((x <= 109) && (y == 113))||
      ((x == 111) && (y > 110))||
	  ((x == 112) && (y == 111))||
	  ((x == 112) && (y == 113)))
	    return REDUCE;
  else{
    if (((x > 101) && (x <= 103) && (y <= 101))||
        ((x > 103) && (x <= 110) && (y <= 103))||
        ((x == 110) && (y > 103) && (y <= 109))||
        ((x == 113) && (y <= 110))||
        ((x <= 110) && (y == 110))||
        ((x <= 110) && (y == 112))||
	    ((x == 113) && (y == 112)))
          return SHIFT;
    else{
        if ((x == 110) && (y == 111))
	      return EQUAL;
		else
		  return ERROR;
	  }
	}
	/*
    if (((x > 10) && (x <= 12) && (y == 10))||
      ((x == 10) && (y == 13))||
	  ((x == 12) && (y == 12))||
      ((x == 10) && (y == 11))||
      ((x == 13) && (y == 11))||
      ((x == 13) && (y == 13)))	   
	    return ERROR;
   */
}

int parse_expression(struct htab_item **expected_item){/*Precedencni syntakticka analyza vyrazu*/
  printf("\n\nPrecedencni syntakticka analyza vyrazu\n");
  /*Inicializace zasobniku*/
  
  int result, error;
  struct htab_item *item;
  
  do{
    if ((result = table_symbols(STop(Stack), token, Stack)) == SYNTAX_ERROR) return SYNTAX_ERROR;
    switch (result){
   
      case SHIFT:
	    switch (token){
          case ID:
	      case INTEGER:
		  case STRING:
	      case BOOLEAN:
	      case DES_INT:
	      case DES_EXP:
	      case DES_EXP_NEG:
	      case EXP:
	      case EXP_NEG:
		    if((error = value(&item)) != SYNTAX_OK) return error;/*Funkce vrati ukazatel na polozku, pokud se jedna o ID, najde v tabulce, jinak vytvori novou jedinecnou polozku*/
			break;
			
		  default:
		    item = NULL;/*Nejedna se o hodnotu*/
	        break;
	      }
		  printf("\nSHIFT");
		  if ((SPush(Stack, token, item)) != INTERNAL_OK) return INTERNAL_ERR;
		  printf("\nDalsi token bude:");
		  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	    break;
	  
      case REDUCE:
	    printf("\nREDUCE");
	    switch (STop(Stack)){
		  /*Zpracovani hodnoty, pouze se prepise na vyraz*/
	      case ID:
	      case INTEGER:
	      case STRING:
	      case BOOLEAN:
	      case DES_INT:
	      case DES_EXP:
	      case DES_EXP_NEG:
	      case EXP:
	      case EXP_NEG:
            Stack -> Top -> data = EXPRESSION;
		    break;
		
	      case MUL:
	      case DIV:
          case ADD:
          case DIF:
          case S:
          case L:
          case SE:
          case LE:
          case EQ:
          case SL:
		    ;
		    struct htab_item *operand_1;
			struct htab_item *operand_2;
			int operator;
			int type_of_result;
			
			
            operand_2 = Stack -> Top -> item;/*Druhy operand bude prvni na zasobniku*/ 			
            SPop(Stack);/*Vyhodime operand ze zasobniku*/
			operator = Stack -> Top -> data;/*Zapamatujeme si operator*/
		    SPop(Stack);/*Vyhodime operator ze zasobniku*/
			operand_1 = Stack -> Top -> item;/*Zapamatujeme si prvni operand*/
			SPop(Stack);/*Vyhodime prvni operand ze zasobniku*/
			
			/*Vytvorime novou promenou, pro ni vytvorime polozku v tabulce a jeji typ nastavime na typ vysledku, kterou nam da funkce type_control*/
			string new_variable;
			strInit(&new_variable);
			generateVariable(&new_variable);
			if ((item = add_var(new_variable.str, table, &error)) == NULL){
			  strFree(&new_variable);
			  return error;
			  }
			strFree(&new_variable);
			
			/*Zavolame funkci type_control, ktera zkontroluje typy a vrati typ vysledku*/
			/*Pokud vrati funkce type_control SEM_ERROR_TYPE, tak nastala semanticka chyba*/
            if ((type_of_result = type_control(&operand_1, operator, &operand_2)) == SEM_ERROR_TYPE){
				return SEM_ERROR_TYPE;
                }
            
			item -> type = type_of_result;/*Nove polozcce priradime typ*/
			  			
			//generateInstruction - switch podle operatoru
			
			switch (operator){
			  case MUL:
			    if ((generateInstruction(I_MUL, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
	          case DIV:
			    if ((generateInstruction(I_DIV, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
              case ADD:
			    if ((generateInstruction(I_ADD, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
              case DIF:
			    if ((generateInstruction(I_SUB, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
              case S:
			    if ((generateInstruction(I_SMALL, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
              case L:
			    if ((generateInstruction(I_GREAT, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
              case SE:
			    if ((generateInstruction(I_SMEQ, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
              case LE:
			    if ((generateInstruction(I_GREQ, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
              case EQ:
			    if ((generateInstruction(I_EQUAL, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
              case SL:
			    if ((generateInstruction(I_NONEQ, (void *) operand_1, (void *) operand_2, (void *) item)) == INTERNAL_ERR) return INTERNAL_ERR;
			    break;
			  
			  default:
			    break;
			}
			
			
            if ((SPush(Stack, EXPRESSION, item)) != INTERNAL_OK) return INTERNAL_ERR;
		    break;  
		  
		  case R_BRACKET:/*Zpracovavame vyraz (EXPRESSION), musime se zbavit zavorek*/
		    ;
			struct htab_item *item;
			
			SPop(Stack);/*Vyhodime pravou zavorku*/
			item = Stack -> Top -> item; /*Zapamatujem si vyraz*/
			SPop(Stack);/*Vyhodime vyraz*/
			SPop(Stack);/*Vyhodime levou zavorku*/
			if ((SPush(Stack, EXPRESSION, item)) != INTERNAL_OK) return INTERNAL_ERR;/*Puvodni vyraz nahrajem zpatky*/
			break;
		  
		  default:
            break;		  
	      }
	    break;
		
		
	  /*Jsme ve stavu (E, potrebujeme pridat pravou zavorku*/
	  case EQUAL:
	    if ((SPush(Stack, token, NULL)) != INTERNAL_OK) return INTERNAL_ERR;/*Pridame pravou zavorku*/
	    if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	    break;
	  
	  case ERROR:
	    SEmpty(Stack);
	    return SYNTAX_ERROR;
	    break;
	  
	  default:
	    break;
      }
    }while (!((STop(Stack) == DOLLAR) && ((token == THEN) || (token == DO) || (token == SEMICOLON) || (token == END))));
  printf("\n\nVyprazdneni zasobniku");
  if ((*expected_item) == NULL){ /*Pozaduju navratit vysledek hodnoty bool, znamena to ze je funkce volana s if nebo while*/
	if (Stack -> Top -> item -> type != s_boolean){
	  printf("\nPodminka pozaduje typ bool");
	  return SEM_ERROR;
	  }
    }	  
  else{	
    if (Stack -> Top -> item -> type != (*expected_item) -> type){
      printf("\nTyp vyrazu se neshoduje s promenou do ktere prirazujeme");
	  return SEM_ERROR;
	  }
    }
  (*expected_item) = Stack -> Top -> item;/*Vratime ukazatel na polozku, ve ktere bude vysledny vyraz*/
  
  SEmpty(Stack);
  printf("\n");
  printf("\nKonec precedencni syntakticke analyzy vyrazu");
  printf("\n");
  return SYNTAX_OK;  
  
}

int type_control(struct htab_item** operand_1, int operator, struct htab_item** operand_2){
  /*Typ operatoru (operator -> type) oznacuji cisla, a to nasledovne:
    0..integer
	1..real
	2..string
	3..boolean
	4..default (typ nebyl nastaven)
  */
  
  int type_of_result = s_default;
  
  if ((*operand_1) -> type == s_default || (*operand_2) -> type == s_default)/*Tato situace nemuze prakticky nastat*/
    return SEM_ERROR_TYPE;
  
  /*Zkontrolujeme, zda s danyma operandame, muzeme provest danou operaci*/
  switch (operator){
    
	/*NASOBENI*/
	case MUL:
	  /*Druhy, ani prvni operand nesmi byt retezec, nebo boolean*/
	  switch ((*operand_1) -> type){
	    /*INTIGER*/
		case 0:	
          if ((*operand_2) -> type == s_integer)
		    type_of_result = s_integer;
		  else{	
		    if ((*operand_2) -> type == s_real)
		      type_of_result = s_real;
			else
			  return SEM_ERROR_TYPE;
			  }
          break;			
		/*REAL*/
		case 1:
		  if ((*operand_2) -> type == s_integer || (*operand_2) -> type == s_real)
		    type_of_result = s_real;
		  else
		    return SEM_ERROR_TYPE;
          break;			 

		default:
		  return SEM_ERROR_TYPE;
		  break;
	    }	
	  break;
	
	/*DELENI*/  
	case DIV:
	  /*Druhy, ani prvni operand nesmi byt retezec, nebo boolean*/
	  switch ((*operand_1) -> type){
	    /*INTIGER*/
	    case 0:				
		/*REAL*/
		case 1:
		  if ((*operand_2) -> type == s_integer || (*operand_2) -> type == s_real)
		    type_of_result = s_real;/*Vysledek deleni je vzdy real*/
	      else
		    return SEM_ERROR_TYPE;
          break;			 
		
		default:
		  return SEM_ERROR_TYPE;
		  break;
	    }
	  break;
	
	/*SCITANI*/
    case ADD:
	  /*Druhy, ani prvni operand nesmi byt boolean*/
	  switch ((*operand_1) -> type){
	    /*INTIGER*/
	    case 0:	
          if ((*operand_2) -> type == s_integer)
            type_of_result = s_integer;
          else{
            if ((*operand_2) -> type == s_real)
			  type_of_result = s_real;
			else
			  return SEM_ERROR_TYPE;
            }
          break;			
		/*REAL*/
		case 1:
		  if ((*operand_2) -> type == s_integer || (*operand_2) -> type == s_real)
		    type_of_result = s_real;
	      else
		    return SEM_ERROR_TYPE;
          break;
        /*STRING*/
        case 2:
          if ((*operand_2) -> type == s_string)/*Konkatenace retezce*/
            type_of_result = s_string;
          else 
            return SEM_ERROR_TYPE;	
          break;			
		
		default:
		  return SEM_ERROR_TYPE;
		  break;
	    }
	  break;
    
    /*MINUS*/	
	case DIF:
	  /*Druhy, ani prvni operand nesmi byt retezec, nebo boolean*/
	  switch ((*operand_1) -> type){
	    /*INTIGER*/
	    case 0:	
          if ((*operand_2) -> type == s_integer)
            type_of_result = s_integer;
          else{
            if ((*operand_2) -> type == s_real)
			  type_of_result = s_real;
			else
			  return SEM_ERROR_TYPE;
            }
          break;			
		/*REAL*/
		case 1:
		  if ((*operand_2) -> type == s_integer || (*operand_2) -> type == s_real)
		    type_of_result = s_real;
	      else
		    return SEM_ERROR_TYPE;
          break;	  
		
		default:
		  return SEM_ERROR_TYPE;
		  break;
	    }
	  break;
	
    /*POROVNAVANI*/	
    case S:
	case L:
    case SE:
    case LE:
    case EQ:
    case SL: 
    /*Pri porovnani musi byt operandy stejneho typu a vysledek je vzdy bool*/	
	  switch ((*operand_1) -> type){
	    /*INTIGER*/
	    case 0:	
          if ((*operand_2) -> type != s_integer)
            return SEM_ERROR_TYPE;
          break;			
		/*REAL*/
		case 1:
		  if ((*operand_2) -> type != s_real)
		    return SEM_ERROR_TYPE;
          break;
        /*STRING*/
        case 2:
          if ((*operand_2) -> type != s_string)
            return SEM_ERROR_TYPE;
          break;		  
		
		case 3:
		  if ((*operand_2) -> type != s_boolean)
		    return SEM_ERROR_TYPE;
		  break;
		  
		default:
		  return SEM_ERROR_TYPE;
		  break;
	    }
	  type_of_result = s_boolean;/*Vysledek porovnani je vzdy bool*/	
	  break;
    }
  /*Pokud kontrola probehla spravne, vratime typ vysledku*/
  return type_of_result;
}









/*

Funkce potrebne pro operace nad zasobnikem
Zasobnik se pouziva k precedencni syntakticke analyze


*/


/*Vlozeni prvku na zasobnik*/

void SInit(PtrStack Stack){

  if (Stack != NULL)
    Stack -> Top = NULL; 

}

int SPush(PtrStack Stack, int data, struct htab_item *item){

  PtrElement tmp;
  printf("\n**push ");
  vypis(data);
  
  if ((tmp = (PtrElement) malloc(sizeof(struct Element))) == NULL) return INTERNAL_ERR;
  tmp -> data = data;
  tmp -> item = item;
  tmp -> Next = Stack -> Top;
  Stack -> Top = tmp;
  return INTERNAL_OK;

}

int STopExpression(PtrStack Stack){
  
  if (Stack -> Top != NULL){
    if (Stack -> Top -> data == EXPRESSION){
      return PRAVDA;
	  }
    else
      return NEPRAVDA;
	}
  else{ 
	return NEPRAVDA;
	}

}

/*Odebrani prvku ze zasobniku*/
void SPop(PtrStack Stack){
  
  PtrElement tmp;

  if (Stack -> Top != NULL){
    tmp = Stack -> Top;
	printf("\n**pop ");
	vypis(Stack -> Top -> data);
    Stack -> Top = Stack -> Top -> Next;
	free(tmp);
	}
   else
     printf("\nnemam co popnout");
}

/*Precteni hodnoty, ktera je na vrcholu zasobniku*/
int STop(PtrStack Stack){

  if (Stack -> Top == NULL)
    return DOLLAR;
  else{
    if (Stack -> Top -> data == EXPRESSION){/*Pokud je na vrcholu zasobniku zpracovany vyraz vrati dalsi polozku*/
      if (Stack -> Top -> Next != NULL)
	    return Stack -> Top -> Next -> data;
	  else 
        return DOLLAR;
      }		
    else
	  return Stack -> Top -> data;;
	}

}

/*Vyprazdneni zasobniku*/
void SEmpty(PtrStack Stack){

  if (Stack -> Top != NULL){
    SPop(Stack);/*Volame Pop dokud neodeberem vsechny prvky*/
    SEmpty(Stack);
    }

}







