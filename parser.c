#include <stdio.h>
#include "str.h"
#include "stable.h"
#include "ilist.h"
#include "scaner.h"
#include "parser.h"

int token;/*Globalni promena*/

int parse()
{
  printf("ahoj_parse\n");
  int result;
  strInit(&attr);
  if ((token = getNextToken(&attr)) == LEX_ERROR)
     result = LEX_ERROR;
  else
     result = program();
  strFree(&attr);
  return result;
}

int program(){ /*<PROGRAM>*/
  
  token = getNextToken();
  
  switch (token){
    /*<PROGRAM> -> <DECLARATION> <FUNCTION> <BODY> DOT*/
    case VAR:
    case FUNCTION:
    case BEGIN:
      
	  if ((declaration()) != SYNTAX_OK) return SYNTAX_ERROR;/*<DECLARATION>*/  
	  if ((function()) != SYNTAX_OK) return SYNTAX_ERROR;/*<FUNCTION>*/	  
	  if ((body()) != SYNTAX_OK) return SYNTAX_ERROR;/*BODY*/  
	  if (token != DOT)/*DOT*/ 
	    return SYNTAX_ERROR;
	  else	  
	    if ((getNextToken()) != END_OF_FILE) return SYNTAX_ERROR;
	  
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
	  if ((getNextToken()) != ID) return SYNTAX_ERROR;
	  if ((getNextToken()) != COLON) return SYNTAX_ERROR;
	  token = getNextToken();
      if ((type()) != SYNTAX_OK) return SYNTAX_ERROR;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  token = getNextToken();
	  if ((n_declaration()) != SYNTAX_OK) return SYNTAX_ERROR;
      return SYNTAX_OK;	      
	  break;
	
	/*<DECLARATION> -> eps*/
	case FUNCTION:
	case BEGIN:
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
	  token = getNextToken();/*Pokud dane pravidlo pokryje nejaky token, vola dalsi*/
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
	  if ((getNextToken()) != COLON) return SYNTAX_ERROR;
	  token = getNextToken();
	  if ((type()) != SYNTAX_OK) return SYNTAX_ERROR;
	  if (token != SEMICOLON) return SYNTAX_ERROR;
	  token = getNextToken();
	  if ((n_declaration()) != SYNTAX_OK) return SYNTAX_ERROR;
	  return SYNTAX_OK;
	  break;

	/*<N_DECLARATION> -> eps*/
	case FUNCTION:
	case BEGIN:
	  return SYNTAX_OK;
	  break;
	
    default:
      return SYNTAX_ERROR;
  }	  
}

int function(){

}

int body(){/*<BODY>*/
  
  switch (token){
    /*<BODY> -> BEGIN <ELEMENT> END*/
    case BEGIN:
	  token = getNextToken();
	  if ((element()) != SYNTAX_OK) return SYNTAX_ERROR;
	  if (token != END) return SYNTAX_ERROR;
	  token = getNextToken();
	  return SYNTAX_OK;
	break;
	
	default:
	  return SYNTAX_ERROR;
  }
}

int element(){/*<ELEMENT>*/
  /*<ELEMENT> -> eps*/
  if (token == END)
    return SYNTAX_OK;
  else{
    /*<ELEMENT> -> <SELECT_ELEMENT> <N_ELEMENT>*/  
    if ((select_element()) != SYNTAX_OK) return SYNTAX_ERROR;
    if ((n_element()) != SYNTAX_OK) return SYNTAX_ERROR;
	return SYNTAX_OK;
    }	
}

int select_element(){/*SELECT_ELEMENT*/

  

}














