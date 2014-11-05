    //Martin Knotek xknote11

    // lexikalni analyzator
    #include <stdio.h>
    #include <ctype.h>
    #include <stdlib.h>
    #include "str.h"
    #include "scaner.h"

    // promenna pro ulozeni vstupniho souboru
    FILE *source;
    short obs = 0;   //kontorla, zda ciselne exponentu, nebo cisla obsahuji nejakou hodnotu
    string pom;

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
             if (isspace(c))    //bila mista ignorujeme
                state = 0;
             else
             if (c == '{')  // komentar
                state = 1;
             else
             if(isalpha(c)) //jedna se o cislo - int/double
             {
                 strAddChar(attr, c);
                 state = 6;
             }
             if ((isalnum(c))||(c == '_'))  //jedna se o slovo
             {
                strAddChar(attr, c);
                state = 2;
             }
             else
             if (c == '+')
                state = 3;
             else
             if (c == '-')
                state = 3;
             else
             if (c == '*')
                state = 3;
             else
             if (c == '/')
                state = 3;
             else
             if (c == '>')
                state = 5;
             else
             if (c == '<')
                state = 4;
             else
             if (c == '=')
                state = 3;
             else
             if(c == ':')
                state = 12;
             if(c == APS)    //39 = ascii hodnota apostrofu
                state = 13;
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
             if (isalnum(c)||(c == '_'))
             {
                strAddChar(attr, c);
             }
             else
             // konec identifikatoru
             {
                ungetc(c, source); // POZOR! Je potreba vratit posledni nacteny znak

                // kontrola, zda se nejedna o klicove slovo
                /* lepsi bude implementace pomoci hash tab  */
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
             if ((c == '_')||(isalnum(c)))
                {
                    ungetc(c, source);
                    /* SPATNE - unget nevrati o dva znaky zpet */
                    if(c == '+') return ADD;
                    else
                    if(c == '-') return DIF;
                    else
                    if(c == '*') return TIM;
                    else
                    if(c == '/') return DIV;
                    else
                    if(c == '=') return EQ;
                }
             else return LEX_ERROR;
           break;

           case 4:
            //mensi
            if ((c == '_')||(isalnum(c)))
            {
                ungetc(c, source);
                return S;//mensitko
            }
            else
                if(c == '=') return SE;//mensi nebo rovno
                else
                if(c == '>') return SL;//nerovno
                else return LEX_ERROR;
            break;

           case 5:
               //vetsi
            if((c == '_')||(isalnum(c)))
            {
                ungetc(c, source);
                return L;//vetsitko
            }
            else
                if(c == '=') return LE;//vetsi nebo rovno
                    else return LEX_ERROR;
            break;

           case 6:
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                }
                else
                {
                    if(c == '.')
                    {
                        strAddChar(attr, c);
                        state = 7;
                    }
                    else
                    {
                        if(c == 'e' || c == 'E')
                        {
                            strAddChar(attr, c);
                            state = 10;
                        }
                        else
                        {
                            if(isalpha(c) && ((c != ' ') || (c != ';'))) return LEX_ERROR;
                            else
                            {
                                ungetc(c, source);
                                return INTEGER;
                            }
                        }
                    }
                }

            break;

           case 7:
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
                else
                {
                    if(obs == 0) return LEX_ERROR;
                    if(c == 'e' || c == 'E')
                    {
                        strAddChar(attr, c);
                        state = 8;
                    }
                    else
                    {
                        if(isalpha(c) && ((c != ' ') || (c != ';'))) return LEX_ERROR;
                        else
                        {
                            ungetc(c, source);
                            obs = 0;
                            return DES_INT;
                        }
                    }
                }
            break;

           case 8:
               if(c>='0' && c<='9'))
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
               else
               {
                   if(obs == 0) return LEX_ERROR;
                   if(c == '-')
                   {
                       strAddChar(attr, c);
                        state = 9;
                   }
                   else
                   {
                       if(c == '+') state = 8;
                       else
                       {
                           if(isalpha(c) && ((c != ' ') || (c != ';'))) return LEX_ERROR;
                            else
                            {
                                ungetc(c, source);
                                obs = 0;
                                return DES_EXP;
                            }
                       }
                   }
               }
            break;

           case 9:
               if(c>='0' && c<='9'))
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
                else
                {
                    if(isalpha(c) && ((c != ' ') || (c != ';'))) return LEX_ERROR;
                    else
                    {
                        ungetc(c, source);
                        obs = 0;
                        return DES_EXP_NEG;
                    }
                }
            break;

           case 10:
               if(c>='0' && c<='9'))
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
               else
               {
                   if(obs == 0) return LEX_ERROR;
                   if(c == '-')
                   {
                       strAddChar(attr, c);
                        state = 11;
                   }
                   else
                   {
                       if(c == '+') state = 10;
                       else
                       {
                           if(isalpha(c) && ((c != ' ') || (c != ';'))) return LEX_ERROR;
                            else
                            {
                                ungetc(c, source);
                                obs = 0;
                                return EXP;
                            }
                       }
                   }
               }
            break;

           case 11:
               if(c>='0' && c<='9'))
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
                else
                {
                    if(isalpha(c) && ((c != ' ') || (c != ';'))) return LEX_ERROR;
                    else
                    {
                        ungetc(c, source);
                        obs = 0;
                        return EXP_NEG;
                    }
                }
            break;

           case 12:
               if(c == '=')
                return ASS;
               else return LEX_ERROR;
            break;

           case 13:
               /* rozsirit o povolene/nepovolene znaky  */
               if(isalnum(c))
               {
                   strAddChar(attr; c);
               }
               if(c == APS)  //APS = 39 = ascii hodnota apostrofu
               {
                   state = 14;
               }
            break;

           case 14:
               if(c == '\n')
               {
                   return STRING;
               }
               if(c == APS)
               {
                   strAddChar(attr, APS);
                   state = 13;
               }
               if(c == '#')
               {
                   state = 15;
               }
               else return LEX_ERROR;
            break;

           case 15:
               if(c>='0' && c<= '9')
                {
                    strAddChar(pom, c);
                }
                if(c == APS)
                {
                    strAddChar(attr, atoi(pom));
                    state = 13;
                }
                else return LEX_ERROR;
            break;
        }
      }
    }
