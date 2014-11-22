#include <stdio.h>
#include "str.h"
#include "scaner.h"
#include "parser_declaration.h"

int token;/*Globalni promena*/
int parse();
int program();
int function();
int body();
int declaration();
int n_declaration();
int type();
string attr;

int parse()
{
  int result;
  strInit(&attr);
  if ((token = getNextToken(&attr)) == LEX_ERROR)
     // nastala chyba jiz pri nacteni prvniho lexemu
     result = LEX_ERROR;
  else
     result = program();
  strFree(&attr);
  return result;
}

int program(){ /*<PROGRAM>*/
  
  switch (token){
    /*<PROGRAM> -> <DECLARATION> <FUNCTION> <BODY> DOT*/
    case VAR:
    case FUNCTION:
    case BEGIN:
      
	  if ((declaration()) != SYNTAX_OK) return SYNTAX_ERROR;/*<DECLARATION>*/  
	  if ((function()) != SYNTAX_OK) return SYNTAX_ERROR;/*<FUNCTION>*/	  
	  if ((body()) != SYNTAX_OK) return SYNTAX_ERROR;/*BODY*/  	  
	  if (token != END_OF_FILE) return SYNTAX_ERROR;
	  printf("END_OF_FILE\n");
	  
	  return SYNTAX_OK;/*Pokud zadne z pravidel nevrati SYNTAX_ERROR*/
	  
	  break;
	
	default:/*Pokud je prvni token jiny nez povoleny*/
      return SYNTAX_ERROR;	
	}	
}

int declaration(){/*<DECLARATION>*/

  switch (token){
    /*<DECLARATION> -> VAR ID COLON <TYPE> SEMICOLON <N_DECLARATION>*/
	case VAR:
	  printf("VAR\n");
	  if ((getNextToken(&attr)) != ID) return SYNTAX_ERROR;
	  printf("ID\n");
	  if ((getNextToken(&attr)) != COLON) return SYNTAX_ERROR;
	  printf("COLON\n");
	  token = getNextToken(&attr);
      if ((type()) != SYNTAX_OK) return SYNTAX_ERROR;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  printf("SEMICOLON\n");
	  token = getNextToken(&attr);
	  if ((n_declaration()) != SYNTAX_OK) return SYNTAX_ERROR;
      return SYNTAX_OK;	      
	  break;
	
	/*<DECLARATION> -> eps*/
	case FUNCTION:
	case BEGIN:
	  printf("FUNCTION or BEGIN\n");
	  return SYNTAX_OK;
	  break;
	  
	default:/*Nemuze nastat, ale z konvence to pridavam*/
	  return SYNTAX_ERROR;
    }
}

int type (){/*<TYPE>*/

  switch (token){
    /*<TYPE> -> T_INTEGER, pro ostatni case analogicky*/  
    case T_INTEGER:
	case T_REAL:
	case T_STRING:
	  printf("T_INTEGER or T_REAL or T_STRING\n");
	  token = getNextToken(&attr);/*Pokud dane pravidlo pokryje nejaky token, vola dalsi*/
	  return SYNTAX_OK;
	  break;
	  
	default:
      return SYNTAX_ERROR;
    }	  
}

int n_declaration(){/*<N_DECLARATION>*/
  
  switch (token){
    /*<N_DECLARATION> -> ID COLON <TYPE> SEMICOLON <N_DECLARATION>*/
    case ID:
	  printf("ID\n");
	  if ((getNextToken(&attr)) != COLON) return SYNTAX_ERROR;
	  printf("COLON\n");
	  token = getNextToken(&attr);
	  if ((type()) != SYNTAX_OK) return SYNTAX_ERROR;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  printf("SEMICOLON\n");
	  token = getNextToken(&attr);
	  if ((n_declaration()) != SYNTAX_OK) return SYNTAX_ERROR;
	  return SYNTAX_OK;
	  break;

	/*<N_DECLARATION> -> eps*/
	case FUNCTION:
	case BEGIN:
	  printf("FUNCTION or BEGIN\n");
	  return SYNTAX_OK;
	  break;
	
    default:
      return SYNTAX_ERROR;
    }	  
}

int function(){/*<FUNCTION>*/
  return SYNTAX_OK;
}

int body(){/*<BODY>*/
  return SYNTAX_OK;  
}

