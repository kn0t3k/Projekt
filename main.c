#include <stdio.h>
#include "err.h"
#include "str.h"
/*#include "stable.h"
#include "ilist.h"*/
#include "scaner.h"
#include "parser.h"
//#include "interpret.h"


int main(int argc, char** argv)
{
   FILE *f;
   if (argc == 1)
   {
      fprintf(stderr, "Neni zadan vstupni soubor\n");
      return INTERNAL_ERR;
   }
   if(argc > 2)
   {
       fprintf(stderr, "Spatne parametry prikazove radky\n");
       return INTERNAL_ERR;
   }
   if ((f = fopen(argv[1], "r")) == NULL)
   {
      fprintf(stderr, "Soubor se nepodarilo otevrit\n");
      return INTERNAL_ERR;
   }

   setSourceFile(f);
   /*tSymbolTable ST;
   tableInit(&ST); // inicializace tabulky symbolu*/

   /*tListOfInstr instrList;
   listInit(&instrList); // inicializace seznamu instrukci*/

   int result;

   result = parse(); // provedeme syntaktickou analyzu


   switch (result)
   {
     case LEX_ERROR:
         printf("err LEX \n");
         //tiskni_radky();
         return -result;
     break;
     case SYNTAX_ERROR:
         printf("err STX \n");
         //tiskni_radky();
         return -result;
     break;
     case SEM_ERROR:
         //tiskni_radky();
         printf("err SYM \n");
       // nastala chyba v prubehu prekladu
       /*tableFree(&ST);
       listFree(&instrList);
       fclose(f);*/
         return -result;
     break;
     // jinak probehlo vse v poradku, muzeme provadet kod
   }
   printf("\n**vse OK**\n");

   // provedeme interpretaci
   /*inter(&ST, &instrList);

   tableFree(&ST);
   listFree(&instrList);*/
   fclose(f);
   return 0;
}
