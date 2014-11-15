int scaner(){
  actual_token = getNextToken(&attr);
  while(1){
    
	next_token = getNextToken(&attr);
	
	switch (actual_token){
	  
	  case VAR:
	  case FUNCTION:
	    if(next_token != ID)
		  return SYNTAX_ERROR;
	  break;
	  
	  case ID:
	    if((next_token != COLON) || (next_token != ASS) || (next_token != R_BRACKET))
		  return SYNTAX_ERROR;
	  break;
	  
	  case COLON:
	    if((next_token != INTEGER) || (next_token != REAL) || (next_token != STRING) || (next_token != BOOLEAN))
		  return SYNTAX_ERROR;
	  break;

	  case ASS:
	    if((next_token != ASSIGNED_STRING) || (next_token != ASSIGNED_INTEGER) || (next_token != DES_INT) || 
		(next_token != DES_EXP) || (next_token != DES_EXP_NEG) || (next_token != EXP) || (next_token != EXP_NEG))
		  return SYNTAX_ERROR;
		break;
	  
      case INTEGER:
      case REAL:
      case STRING:
      case BOOLEAN:
        if(next_token != SEMICOLON)
          return SYNTAX_ERROR;		
	  
	  case ASSIGNED_STRING:
	    if((next_token != SEMICOLON) || (next_token != END) || (next_token != R_BRACKET))
		  return SYNTAX_ERROR;
	  
	  case ASSIGNED_INTEGER:
	  case DES_INT:
	  case DES_EXP:
	  case DES_EXP_NEG:
	  case EXP:
	  case EXP_NEG:
	    if((next_token != SEMICOLON) || (next_token != END))
		  return SYNTAX_ERROR;  
	  
	  case SEMICOLON:
	    if((next_token != ID) || (next_token != FUNCTION) || (next_token != BEGIN) || (next_token != WHILE) || 
		(next_token != READLN) || (next_token != WRITE))
          return SYNTAX_ERROR;
	  break;
	  
	  case BEGIN:
	    if((next_token != ID) || (next_token != IF) || (next_token != WHILE) || (next_token != WRITE) ||
		(next_token != READLN))
		  return SYNTAX_ERROR;
	  break;
	  
	  case THEN:
	  case ELSE:
	    if(next_token != BEGIN)
		  return SYNTAX_ERROR;
	  break;

      case WRITE:
	  case READLN:
	    if(next_token != L_BRACKET)
		  return SYNTAX_ERROR;
	  break;
	  
	  case L_BRACKET:
	    if((next_token != ID) || (next_token != ASSIGNED_STRING))
		  return SYNTAX_ERROR;
	  break;
      
	  
      }	  
    actual_token = next_token;
    }

}
