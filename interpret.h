/*--------------------------------------
| Projekt: IFJ14						
| Soubor: interpret_2.h						
| Autori: Denk Filip (xdenkf00)		
|	  Jurica Tomas (xjuric22)		
|	  Knotek Martin (xknote11)	
|	  Kohut Jan (xkohut08)		
|	  Piskula David (xpisku02)	
| Prekladac: GCC 4.8.2-7				
| Datum:   5.12.2014					
|--------------------------------------*/




#define INT_INT 330
#define INT_REAL 331
#define INT_STRING 332
#define INT_BOOLEAN 333
#define I_LABEL 331
#define I_IFGOTO 332
#define I_GOTO 333
#define I_END 334
#define I_ADD 335
#define I_SUB 336
#define I_MUL 337
#define I_DIV 338
#define I_INC 339
#define I_DEC 310
#define I_GREAT 311
#define I_SMALL 312
#define I_EQUAL 313
#define I_GREQ 314
#define I_SMEQ 315
#define I_NONEQ 316
#define I_ASSIGN 317
#define I_PUSH_PARAM 318
#define I_CALL 319
#define I_RETURN 320
#define I_WRITE 321
#define I_COPYVAR 322


int interpret(symbol_table_item *GTable, tList *LS);
