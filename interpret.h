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




#define I_FIND 323
#define I_SORT 324
#define I_COPY 325
#define I_LENGTH 326
#define I_LABEL 331
#define I_IFGOTO 332
#define I_GOTO 333
#define I_END 334
#define I_ADD 335
#define I_SUB 336
#define I_MUL 337
#define I_DIV 338
#define I_READ 339
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


#define SEM_ERROR    -3

int interpret(symbol_table_item *GTable, tList *LS);
