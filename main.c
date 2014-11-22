#include <stdio.h>
#include "str.h"
/*#include "stable.h"
#include "ilist.h"*/
#include "scaner.h"
#include "parser_declaration.h"
//#include "interpret.h"

#define INTERNAL_ERR 99


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
     case SYNTAX_ERROR:
     case SEM_ERROR:
       // nastala chyba v prubehu prekladu
       /*tableFree(&ST);
       listFree(&instrList);
       fclose(f);*/
        printf("\n**chyba**\n");
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
