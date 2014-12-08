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

   int result;

   result = parse(table, &list, Stack); // provedeme syntaktickou analyzu
   
   SEmpty(Stack);/*Zasobnik se vyprazdni, pokud doslo k chybe nekde v prubehu precedencni syntakticke analyzy*/   
   free(Stack);
	 
   switch (result)
   {
     case LEX_ERROR:
         printf("\nerr LEX \n");
         //tiskni_radky();
         return -result;
     break;
     case SYNTAX_ERROR:
         printf("\nerr STX \n");
         //tiskni_radky();
         return -result;
     break;
     case SEM_ERROR:
         //tiskni_radky();
         printf("\nerr SEM \n");
       // nastala chyba v prubehu prekladu
       /*tableFree(&ST);
       listFree(&instrList);
       fclose(f);*/
         return -result;
     break;
	 case SEM_ERROR_TYPE:
         //tiskni_radky();
         printf("\nerr SEM_TYPE \n");
       // nastala chyba v prubehu prekladu
       /*tableFree(&ST);
       listFree(&instrList);
       fclose(f);*/
         return -result;
	 case INTERNAL_ERR:
         //tiskni_radky();
         printf("\nerr INTERNAL_ERR \n");
       // nastala chyba v prubehu prekladu
       /*tableFree(&ST);
       listFree(&instrList);
       fclose(f);*/
         return -result;
	 
     // jinak probehlo vse v poradku, muzeme provadet kod
   }
   printf("\n**vse OK**\n");
   interpret(table -> global, &list);
   symbol_table_free(table);
   DisposeList(&list);
   fclose(f);
   return 0;
}
