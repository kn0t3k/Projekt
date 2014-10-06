// lexikalni analyzator
#include <stdio.h>
#include <ctype.h>
#include "str.h"
#include "scaner.h"

// promenna pro ulozeni vstupniho souboru
FILE *source;

void setSourceFile(FILE *f)
{
  source = f;
}

int getNextToken(string *attr)
// hlavni funkce lexikalniho analyzatoru
{
   int state = 0;
   int c;
   // vymazeme obsah atributu a v pripade identifikatoru
   // budeme postupne do nej vkladat jeho nazev
   strClear(attr);
   while (1)
   {
     // nacteni dalsiho znaku
     c = getc(source);

     switch (state)
     {
       case 0:
       // zakladni stav automatu
         if (isspace(c))
         // bila mista - ignorovat
	    state = 0;
         else
         if (c == '{')
         // komentar
            state = 1;
         else
         if (isalpha(c))
         // identifikator nebo klicove slovo
         {
            strAddChar(attr, c);
            state = 2;
         }
         else
         if (c == '+')
         // operator +
            state = 3;
         else
         if (c == '-')
         // operator -
            state = 4;
         else
         if (c == '*')
         // operator *
            state = 5;
         else
         if (c == '/')
         // operator deleno
            state = 6;
         else
         if (c == '>')
            state = 7;
         else
         if (c == '<')
            state = 8;
         else
         if (c == '=')
            state = 9;
         else
         if (c == '{') return LEFT_VINCULUM;
         else
         if (c == '}') return RIGHT_VINCULUM;
         else
         if (c == ';') return SEMICOLON;
         else
         if (c == EOF) return END_OF_FILE;
         else
         return LEX_ERROR;
        break;


       case 1:
       // komentar
         if (c == '}') state = 0;
         else
         // komentar pokracuje libovolnymi znaky, zustan ve stavu 1,
         // ale kontroluj, zda neprijde EOF (neukonceny komentar)
         if (c == EOF) return LEX_ERROR;
       break;


       case 2:
       // identifikator nebo klicove slovo
         if (isalnum(c))
         // identifikator pokracuje
            strAddChar(attr, c);
         else
         // konec identifikatoru
        {
       	    ungetc(c, source); // POZOR! Je potreba vratit posledni nacteny znak

            // kontrola, zda se nejedna o klicove slovo
            if (strCmpConstStr(attr, "BEGIN") == 0) return BEGIN;
            else
            if (strCmpConstStr(attr, "BOOLEAN") == 0) return BOOLEAN;
            else
            if (strCmpConstStr(attr, "DO") == 0) return DO;
            else
            if (strCmpConstStr(attr, "ELSE") == 0) return ELSE;
            else
            if (strCmpConstStr(attr, "END") == 0) return END;
            else
            if (strCmpConstStr(attr, "FALSE") == 0) return FALSE;
            else
            if (strCmpConstStr(attr, "FIND") == 0) return FIND;
            else
            if (strCmpConstStr(attr, "FORWARD") == 0) return FORWARD;
            else
            if (strCmpConstStr(attr, "FUNCTION") == 0) return FUNCTION;
            else
            if (strCmpConstStr(attr, "IF") == 0) return IF;
            else
            if (strCmpConstStr(attr, "INTEGER") == 0) return INTEGER;
            else
            if (strCmpConstStr(attr, "READLN") == 0) return READLN;
            else
            if (strCmpConstStr(attr, "REAL") == 0) return REAL;
            else
            if (strCmpConstStr(attr, "SORT") == 0) return SORT;
            else
            if (strCmpConstStr(attr, "STRING") == 0) return STRING;
            else
            if (strCmpConstStr(attr, "THEN") == 0) return THEN;
            else
            if (strCmpConstStr(attr, "TRUE") == 0) return TRUE;
            else
            if (strCmpConstStr(attr, "VAR") == 0) return VAR;
            else
            if (strCmpConstStr(attr, "WHILE") == 0) return WHILE;
            else
            if (strCmpConstStr(attr, "WRITE") == 0) return WRITE;
            else
            // jednalo se skutecne o identifikator
               return ID;
          }
       break;


       case 3:
       // pokracovani operatoru +
         if ((c == '_')||(isalnum(c))) return ADD;
         else
            return LEX_ERROR;
       break;


       case 4:
       // pokracovani operatoru -
         if ((c == '_')||(isalnum(c))) return DIF;
         else
            return LEX_ERROR;
       break;

       case 5:
        //operator nasobeni
        if ((c == '_')||(isalnum(c))) return TIM;
        else
            return LEX_ERROR;
        break;

       case 6:
        //operator deleni
        if ((c == '_')||(isalnum(c))) return DIV;
        else
            return LEX_ERROR;
        break;

       case 9:
        //rovnase
        if((c == '_')||(isalnum(c))) return EQ;
        else
            return LEX_ERROR;
        break;

    }
  }
}
