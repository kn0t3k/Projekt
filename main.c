/*--------------------------------------
| Projekt: IFJ14						
| Soubor: main.c						
| Autori: Denk Filip (xdenkf00)		
|	  Jurica Tomas (xjuric22)		
|	  Knotek Martin (xknote11)	
|	  Kohut Jan (xkohut08)		
|	  Piskula David (xpisku02)	
| Prekladac: GCC 4.8.2-7				
| Datum:   5.12.2014					
|--------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "err.h"
#include "str.h"
#include "ial.h"
#include "scaner.h"
#include "list.h"
#include "parser.h"
#include "interpret.h"


int main(int argc, char** argv)
{

   int error;
   FILE *f;
   if (argc == 1)
   {
      fprintf(stderr, "Neni zadan vstupni soubor\n");
      return -INTERNAL_ERR;
   }
   if(argc > 2)
   {
       fprintf(stderr, "Spatne parametry prikazove radky\n");
       return -INTERNAL_ERR;
   }
   if ((f = fopen(argv[1], "r")) == NULL)
   {
      fprintf(stderr, "Soubor se nepodarilo otevrit\n");
      return -INTERNAL_ERR;
   }

   setSourceFile(f);
   
   struct symbol_table* table = symbol_table_init(&error);
   if (table == NULL)
     return -INTERNAL_ERR;
	 
   PtrStack Stack;
   if ((Stack = (PtrStack) malloc(sizeof(struct StructStack))) == NULL) return -INTERNAL_ERR;


   tList list;
   InitList(&list); // inicializace seznamu instrukci*/
   SInit(Stack);
   int result;

   result = parse(table, &list, Stack); // provedeme syntaktickou analyzu
   
   SEmpty(Stack);/*Zasobnik se vyprazdni, pokud doslo k chybe nekde v prubehu precedencni syntakticke analyzy*/   
   free(Stack);
	 
   switch (result)
   {
     case LEX_ERROR:
       symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
	   break;
     case SYNTAX_ERROR:
       symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
       break;
     case SEM_ERROR:
       symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
       break;
	 case SEM_ERROR_TYPE:
       symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
	   break;
     case SEM_ERROR_OVERFLOW:
       symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
       break;	   
	 case INTERNAL_ERR:
       symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
	   break;
	   
	 default:
	   break;
	 
     // jinak probehlo vse v poradku, muzeme provadet kod
   }
   result = interpret(table -> global, &list);

   switch (result)
   {
	 case INTERNAL_ERR:
       symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
	   break;
     case RUN_INIT_ERROR:
	   symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
	 case RUN_ZERO_ERROR:
	   symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
	 case RUN_OTHER_ERROR:
	   symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
     case STDIN_NUM_ERROR:
	   symbol_table_free(table);
       DisposeList(&list);
       fclose(f);
       return -result;
	 default:
	   break;
	 
   }
   symbol_table_free(table);
   DisposeList(&list);
   fclose(f);
   return 0;
}
