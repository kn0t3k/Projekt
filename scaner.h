//hlavicka pro lexikalni analyzator

//identifikator
#define ID       0


//klicova slova

#define BEGIN 10
#define BOOLEAN    11
#define DO   12
#define ELSE   13
#define END   14
#define FALSE   15
#define FIND   16
#define FORWARD   17
#define FUNCTION   18
#define IF   19
#define INTEGER 20
#define READLN 21
#define REAL 22
#define SORT 23
#define STRING 24
#define THEN 25
#define TRUE 26
#define VAR 27
#define WHILE 28
#define WRITE 29

//operatory
#define ADD     30
#define DIF     31
#define TIM     32
#define DIV     33
#define EQ		34
#define L		35
#define S		36
#define LE		37
#define SE		38
#define SL		39

//jednotlive znaky
#define LEFT_VINCULUM  40 // '{'
#define RIGHT_VINCULUM 41 // '}'
#define SEMICOLON      42 // ';'

//specialni znaky
#define END_OF_FILE    52

//chybove hlasky
#define LEX_ERROR    1

//hlavicka funkce simulujici lexikalni analyzator
void setSourceFile(FILE *f);
int getNextToken(string *attr);
