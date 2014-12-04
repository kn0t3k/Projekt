#include "ial.h"
#include "list.h"

#define INT_INT 0
#define INT_REAL 1
#define INT_STRING 2
#define INT_BOOLEAN 3
#define I_LABEL 1
#define I_IFGOTO 2
#define I_GOTO 3
#define I_END 4
#define I_ADD 5
#define I_SUB 6
#define I_MUL 7
#define I_DIV 8
#define I_INC 9
#define I_DEC 10
#define I_GREAT 11
#define I_SMALL 12
#define I_EQUAL 13
#define I_GREQ 14
#define I_SMEQ 15
#define I_NONEQ 16
#define I_ASSIGN 17
#define I_PUSH_PARAM 18
#define I_CALL 19
#define I_RETURN 20
#define I_WRITE 21


int interpret(symbol_table_item *GTable, tList *L);
