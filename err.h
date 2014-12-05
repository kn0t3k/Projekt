/*--------------------------------------
| Projekt: IFJ14						
| Soubor: err.h						
| Autori:Denk Filip (xdenkf00)		
|	  Jurica Tomas (xjuric22)		
|	  Knotek Martin (xknote11)	
|	  Kohut Jan (xkohut08)		
|	  Piskula David (xpisku02)	
| Prekladac: GCC 4.8.2-7				
| Datum:   5.12.2014					
|--------------------------------------*/

#define SYNTAX_OK     -1
#define SYNTAX_ERROR -2
#define SEM_ERROR    -3
#define SEM_ERROR_TYPE -4

#define SEM_OK 55

#define SHIFT 65
#define REDUCE 69
#define EQUAL 88
#define ERROR 89
#define DOLLAR 78

#define PRAVDA 200
#define NEPRAVDA 201

#define INTERNAL_ERR 99
#define INTERNAL_OK 98

#define ID       0
#define ID_FUNCTION 75

#define BEGIN 10
#define T_BOOLEAN    11
#define DO   12
#define ELSE   13
#define END   14
#define FALSE   15
#define FIND   16
#define FORWARD   17
#define FUNCTION   18
#define IF   19
#define T_INTEGER 20
#define READLN 21
#define T_REAL 22
#define SORT 23
#define T_STRING 24
#define THEN 25
#define TRUE 26
#define VAR 27
#define WHILE 28
#define WRITE 29
#define STRING 72
#define INTEGER 73
#define REAL 74
#define COPY 76
#define LENGHT 77
#define EXPRESSION 79
#define BOOLEAN 80


#define ADD     30
#define DIF     31
#define MUL     32
#define DIV     33
#define EQ		34
#define L		35
#define S		36
#define LE		37
#define SE		38
#define SL		70
#define ASS		71

#define DES_INT 60	//desetinna cisla
#define DES_EXP 61	//desetinna isla s exponentem
#define DES_EXP_NEG 62	//desetina cisla s zapornym exp
#define EXP 63	//cela cisla s exponemtem
#define EXP_NEG 64	//cela cisla se zapornym exp


#define LEFT_VINCULUM  40 // '{'
#define RIGHT_VINCULUM 41 // '}'
#define SEMICOLON      42 // ';'
#define APS				39	//" ' "
#define L_BRACKET  43 // '('
#define R_BRACKET  44 // ')'
#define COLON  45 // ':'
#define COMMA 46 // ','
#define DOT 47  // '.'

//specialni znaky
#define END_OF_FILE    52

//chybove hlasky
#define LEX_ERROR    1


