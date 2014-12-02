#include <stdio.h>
#include <stdlib.h>
#include "str.h"
#include "scaner.h"
#include "parser.h"
#include "err.h"
#include "ial.h"


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
            case LENGHT:printf("LENGHT");break;
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



int token;/*Globalni promena*/
struct symbol_table* table;
PtrStack Stack;
string attr;



 
/*Funkce, ktere predstavuji nonterminaly*/

int parse(struct symbol_table* table_main, PtrStack Stack_main){
  
  int result;
  table = table_main;
  Stack = Stack_main;
  strInit(&attr);
  if ((token = getNextToken(&attr)) == LEX_ERROR)
     result = LEX_ERROR;
  else
     result = program();
  strFree(&attr);
  return result;
}

int program(){ /*<PROGRAM>*/
  
  
  int result;
  
  switch (token){
    /*<PROGRAM> -> <DECLARATION> <FUNCTION> <BODY> DOT*/
    case VAR:
    case FUNCTION:
    case BEGIN:
      result = declaration();
	  if (result != SYNTAX_OK) return result;/*<DECLARATION>*/
      result = function();	  
	  if (result != SYNTAX_OK) return result;/*<FUNCTION>*/
	  result = body();	  
	  if (result != SYNTAX_OK) return result;/*BODY*/  
	  if (token != DOT) return SYNTAX_ERROR;/*DOT*/
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != END_OF_FILE) return SYNTAX_ERROR;
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
	  result = type(item);
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

int type(struct htab_item *item){/*<TYPE>*/

  switch (token){
    /*<TYPE> -> T_INTEGER, pro ostatni case analogicky*/  
    case T_INTEGER:
	  if (item != NULL)
	    item -> type = s_integer;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;/*Pokud dane pravidlo pokryje nejaky token, vola dalsi*/
	  return SYNTAX_OK;
	  break;
	case T_REAL:
	  if (item != NULL)
	    item -> type = s_real;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;/*Pokud dane pravidlo pokryje nejaky token, vola dalsi*/
	  return SYNTAX_OK;
	  break;
	case T_STRING:
	  if (item != NULL)
	    item -> type = s_string;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;/*Pokud dane pravidlo pokryje nejaky token, vola dalsi*/
	  return SYNTAX_OK;
	  break;
	/*case T_BOOLEAN:
	  if (item != NULL)
	    item -> type = s_boolean;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  return SYNTAX_OK;
	  break;*/
	  
	default:
      return SYNTAX_ERROR;
	  break;
  }	  
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
	  result = type(item);
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
  
  switch (token){
    /*<FUNCTION> -> FUNCTION ID_FUNCTION L_BRACKET <PARAMETER> R_BRACKET COLON <TYPE> SEMICOLON <FUNCTION_BODY> <FUNCTION>*/
    case FUNCTION:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != ID_FUNCTION) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != L_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = parameter();
	  if (result != SYNTAX_OK) return result;
	  if (token != R_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != COLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = type(NULL);
	  if (result != SYNTAX_OK) return result;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = function_body();
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

int parameter(){/*<PARAMETER>*/

  int result;
  
  switch (token){
    /*<PARAMETER> -> ID COLON <TYPE> <N_PARAMETER>*/
	case ID: 
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != COLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = type(NULL);
	  if (result != SYNTAX_OK) return result;
	  result = n_parameter();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	
	/*<PARAMETER> -> eps*/
	case R_BRACKET:
	  return SYNTAX_OK;
	  break;
	  
	default: 
	  return SYNTAX_ERROR;
	  break;
    }
}

int n_parameter(){/*<N_PARAMETER>*/

  int result;
  
  switch (token){
    /*<N_PARAMETER> -> SEMICOLON ID COLON <TYPE> <N_PARAMETER>*/
	case SEMICOLON:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != ID) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != COLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = type(NULL);
	  if (result != SYNTAX_OK) return result;
	  result = n_parameter();
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

int function_body(){/*<FUNCTION_BODY>*/

  int result;
  
  switch (token){
    /*<FUNCTION_BODY> -> FORWARD SEMICOLON*/
    case FORWARD:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  return SYNTAX_OK;
	  break;
	
	/*<FUNCTION_BODY> -> <DECLARATION> <BODY> SEMICOLON*/
	case VAR:
	case BEGIN:
	  result = declaration();
	  if (result != SYNTAX_OK) return result;
	  result = body();
	  if (result != SYNTAX_OK) return result;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
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
  
  int result, error;
  struct htab_item *item;
  
  switch (token){
    /*<CALLFUNASS> -> ID ASS <CALLORASS>*/
    case ID:
	  if ((item = search_var(attr.str, table, &error)) == NULL)
        return error;
	  else
	    item -> initialized = 1;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != ASS) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = callorass();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
    
    default:
      return SYNTAX_ERROR;
      break;	  
    }
}

int callorass(){

  int result;
  
  switch (token){
    /*<CALLORASS> -> <EXPRESSION>*/
    case ID:
	case INTEGER:
	case STRING:
	//case BOOLEAN:
	case DES_INT:
	case DES_EXP:
	case DES_EXP_NEG:
	case EXP:
	case EXP_NEG:
	case L_BRACKET:
	  result = expression();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	 
    /*<CALLORASS> -> ID_FUNCTION L_BRACKET <VARIABLE> R_BRACKET*/  
	case ID_FUNCTION:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != L_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = variable();
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


int variable(){

  int result;
  
  switch (token){
    /*<VARIABLE> -> <VALUE> <N_VARIABLE>*/
	case ID:
	case INTEGER:
	case STRING:
	//case BOOLEAN:
	case DES_INT:
	case DES_EXP:
	case DES_EXP_NEG:
	case EXP:
	case EXP_NEG:
	  result = value();
	  if (result != SYNTAX_OK) return result;
	  result = n_variable();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	
	/*<VARIABLE> -> eps*/
	case R_BRACKET:
	  return SYNTAX_OK;
	  break;
	  
	default:
	   return SYNTAX_ERROR;
	   break;  
	}   
}

int value(){
  
  switch (token){
    /*<VALUE> -> INTEGER, analogicky pro zbytek*/
	case INTEGER:
	case STRING:
	//case BOOLEAN:
	case DES_INT:
	case DES_EXP:
	case DES_EXP_NEG:
	case EXP:
	case EXP_NEG:
	case ID:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  return SYNTAX_OK;
      break;
	  
	default:
	  return SYNTAX_ERROR;
	  break;
	}
}

int n_variable(){/*<N_VARIABLE>*/
  
  int result;
  
  switch (token){
    /*<N_VARIABLE> -> COMMA <VALUE> <N_VARIABLE>*/
    case COMMA:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = value();
	  if (result != SYNTAX_OK) return result;
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
  
  switch (token){
    /*<WHILE_CONDITION> -> WHILE EXPRESSION DO <BODY>*/
    case WHILE:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = expression();
	  if (result != SYNTAX_OK) return result;
	  if (token != DO) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = body();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	  
	default:
	  return SYNTAX_ERROR;
	  break;
    }
}

int if_condition(){/*<IF_CONDITION>*/

  int result;
  
  switch (token){
    /*<IF_CONDITION> -> IF EXPRESSION THEN <BODY> ELSE <BODY>*/
	case IF:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = expression();
	  if (result != SYNTAX_OK) return result;
	  if (token != THEN) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = body();
	  if (result != SYNTAX_OK) return result;
	  if (token != ELSE) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  result = body();
	  if (result != SYNTAX_OK) return result;
	  return SYNTAX_OK;
	  break;
	
	default:
	  return SYNTAX_ERROR;
	  break;
	}
}

int function_readln(){/*<FUNCTION_READLN>*/

  switch (token){
    /*<FUNCTION_READLN> -> READLN L_BRACKET ID R_BRACKET*/
	case READLN:
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != L_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != ID) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
	  if (token != R_BRACKET) return SYNTAX_ERROR;
	  if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
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
	  result = variable();
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

int expression(){/*<EXPRESSION>*/

  int result, error;
  struct htab_item* item;

  switch (token){
    case ID:
	  if ((item = search_var(attr.str, table, &error)) == NULL)
        return error;
	  else{
	    if (item -> initialized != 1)
	      return SEM_ERROR;
	    }	
	case INTEGER:
	case STRING:
	//case BOOLEAN:
	case DES_INT:
	case DES_EXP:
	case DES_EXP_NEG:
	case EXP:
	case EXP_NEG:
	case L_BRACKET:
      result = parse_expression();
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
//	case BOOLEAN:
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
    if (((STopExpression(Stack)) == NEPRAVDA) && (STop(Stack) != ID) && (STop(Stack) != R_BRACKET)){
	  return SYNTAX_ERROR;
	  }
    }
	
  if (x <= 109){
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

int parse_expression(){/*Precedencni syntakticka analyza vyrazu*/
  printf("\n\nPrecedencni syntakticka analyza vyrazu\n");
  /*Inicializace zasobniku*/
  SInit(Stack);
  
  int result, error;
  struct htab_item* item;
  
  do{
    if ((result = table_symbols(STop(Stack), token, Stack)) == SYNTAX_ERROR) return SYNTAX_ERROR;
    switch (result){
   
      case SHIFT:
	    printf("\nSHIFT");
	    if ((SPush(Stack, token)) != INTERNAL_OK) return INTERNAL_ERR;
		printf("\nDalsi token bude:");
	    if ((token = getNextToken(&attr)) == LEX_ERROR) return LEX_ERROR;
		if (token == ID){
		  if ((item = search_var(attr.str, table, &error)) == NULL)
            return error;
	      else{
	        if (item -> initialized != 1)
	          return SEM_ERROR;
	        }
		  }
	    break;
	  
      case REDUCE:
	    printf("\nREDUCE");
	    switch (STop(Stack)){
	      case ID:
	      case INTEGER:
	      case STRING:
	//	  case BOOLEAN:
	      case DES_INT:
	      case DES_EXP:
	      case DES_EXP_NEG:
	      case EXP:
	      case EXP_NEG:
            SPop(Stack);
            if ((SPush(Stack, EXPRESSION)) != INTERNAL_OK) return INTERNAL_ERR;
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
		  case R_BRACKET:
            SPop(Stack);
		    SPop(Stack);
			SPop(Stack);
            if ((SPush(Stack, EXPRESSION)) != INTERNAL_OK) return INTERNAL_ERR;
		    break;  
		  
		  default:
            break;		  
	      }
	    break;
	
	  case EQUAL:
	    if ((SPush(Stack, token)) != INTERNAL_OK) return INTERNAL_ERR;
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
  SEmpty(Stack);
  printf("\n");
  printf("\nKonec precedencni syntakticke analyzy vyrazu");
  printf("\n");
  return SYNTAX_OK;  
  
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

int SPush(PtrStack Stack, int data){

  PtrElement tmp;
  printf("\n**push ");
  vypis(data);
  
  if ((tmp = (PtrElement) malloc(sizeof(struct Element))) == NULL) return INTERNAL_ERR;
  tmp -> data = data;
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







