/*--------------------------------------
| Projekt: IFJ14
| Soubor: scaner.c
| Autori: Denk Filip (xdenkf00)
|	  Jurica Tomas (xjuric22)
|	  Knotek Martin (xknote11)
|	  Kohut Jan (xkohut08)
|	  Piskula David (xpisku02)
| Prekladac: GCC 4.8.2-7
| Datum:   5.12.2014
|--------------------------------------*/

    // lexikalni analyzator
    #include <stdio.h>
    #include <ctype.h>
    #include <stdlib.h>
    #include "err.h"
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

    /*
        STACI ZAKOMENTAROVAT SWITCH NIC JINEHO !!!
        */


    int getNextToken(string *attr)//tuto funkci vola praser a ona vola printtoken aby mohla vytisknout tokeny, potom zmenit
    {
        strInit(&pom);
        int vysledek = printToken(attr);

        switch (vysledek)
        {
            case ID: printf("\n**ID");break;
            case SHIFT:printf("\n**SHIFT");break;
            case REDUCE:printf("\n**REDUCE");break;
            case EQUAL:printf("\n**EQUAL");break;
            case DOLLAR:printf("\n**DOLLAR");break;
            case ID_FUNCTION:printf("\n**ID_FUNCTION");break;
            case BEGIN:printf("\n**BEGIN");break;
            case T_BOOLEAN:printf("\n**T_BOOLEAN");break;
            case BOOLEAN:printf("\n**BOOLEAN");break;
            case DO:printf("\n**DO");break;
            case ELSE:printf("\n** ELSE");break;
            case END:printf("\n**END");break;
            case FALSE:printf("\n**FALSE");break;
            case FIND:printf("\n**FIND");break;
            case FORWARD:printf("\n**FORWARD");break;
            case FUNCTION:printf("\n**FUNCTION");break;
            case IF:printf("\n**IF");break;
            case T_INTEGER:printf("\n**T_INTEGER");break;
            case READLN:printf("\n**READLN");break;
            case T_REAL:printf("\n**T_REAL");break;
            case SORT:printf("\n**SORT");break;
            case T_STRING:printf("\n**T_STRING");break;
            case THEN:printf("\n**THEN");break;
            case TRUE:printf("\n**TRUE");break;
            case VAR:printf("\n**VAR");break;
            case WHILE:printf("\n**WHLE");break;
            case WRITE:printf("\n**WRITE");break;
            case STRING:printf("\n**STRING");break;
            case INTEGER:printf("\n**INTEGER");break;
            case REAL:printf("\n**REAL");break;
            case COPY:printf("\n**COPY");break;
            case LENGTH:printf("\n**LENGTH");break;
            case EXPRESSION:printf("\n**EXPRESSION");break;
            case ADD:printf("\n**ADD");break;
            case DIF:printf("\n**DIF");break;
            case MUL:printf("\n**MUL");break;
            case DIV:printf("\n**DIV");break;
            case EQ:printf("\n**EQ");break;
            case L:printf("\n**L");break;
            case S:printf("\n**S");break;
            case LE:printf("\n**LE");break;
            case SE:printf("\n**SE");break;
            case SL:printf("\n**SL");break;
            case ASS:printf("\n**ASS");break;
            case DES_INT:printf("\n**DES_INT");break;
            case DES_EXP:printf("\n**DES_EXP");break;
            case DES_EXP_NEG:printf("\n**DES_EXP_NEG");break;
            case EXP:printf("\n**EXP");break;
            case EXP_NEG:printf("\n**EXP_NEG");break;
            case LEFT_VINCULUM:printf("\n**LEFT_VINCULUM");break;
            case RIGHT_VINCULUM:printf("\n**RIGHT_VINCULUM");break;
            case SEMICOLON:printf("\n**SEMICOLON");break;
            case APS:printf("\n**APS");break;
            case L_BRACKET:printf("\n**L_BRAKET");break;
            case R_BRACKET:printf("\n**R_BRACKET");break;
            case COLON:printf("\n**COLON");break;
            case COMMA:printf("\n**COMA");break;
            case DOT:printf("\n**DOT");break;
        }

        strFree(&pom);
        return vysledek;
    }


    int printToken(string *attr)
    // hlavni funkce lexikalniho analyzatoru
    {

       int state = 0;
       int c;
       int plusko = NEPRAVDA;

       strClear(attr);
       strClear(&pom);


       while (1)
       {
         // nacteni dalsiho znaku
         c = getc(source);

         switch (state)
         {

           case 0:

             if (isspace(c)) state = 0;  //bila mista ignorujeme
             else
             if (c == '{') state = 1;  //komentar
             else
             if(c>='0' && c<='9') //jedna se o cislo - int/double
             {
                 strAddChar(attr, c);
                 state = 6;
             }
             else
             if ((isalpha(c))||(c == '_'))  //jedna se o slovo
             {
                strAddChar(attr, c);
                state = 2;
             }
             else
             if (c == ',') return COMMA;
             else
             if (c == '.') return DOT;
             else
             if (c == '+') return ADD;
             else
             if (c == '-') return DIF;
             else
             if (c == '*') return MUL;
             else
             if (c == '/') return DIV;
             else
             if (c == '=') return EQ;
             else
             if (c == '{') return LEFT_VINCULUM;
             else
             if (c == '}') return RIGHT_VINCULUM;
             else
             if (c == ';') return SEMICOLON;
             else
             if (c == '(') return L_BRACKET;
             else
             if (c == ')') return R_BRACKET;
             else
             if (c == ':') state = 12;
             else
             if (c == EOF) return END_OF_FILE;
             else
             if (c == '>') state = 5;
             else
             if (c == '<') state = 4;
             else
             if(c == ':') state = 12;
             else
             if(c == APS) state = 13;   //APS = 39 = ascii hodnota apostrofu
             else return LEX_ERROR;
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
                ungetc(c, source);
                // kontrola, zda se nejedna o klicove slovo
                if (strCmpConstStr(attr, "BEGIN") == 0) return BEGIN;
                else
                if (strCmpConstStr(attr, "BOOLEAN") == 0) return T_BOOLEAN;
                else
                if (strCmpConstStr(attr, "DO") == 0) return DO;
                else
                if (strCmpConstStr(attr, "ELSE") == 0) return ELSE;
                else
                if (strCmpConstStr(attr, "END") == 0) return END;
                else
                if (strCmpConstStr(attr, "FALSE") == 0) return BOOLEAN;
                else
                if (strCmpConstStr(attr, "FIND") == 0)
                {
                    if((c=getc(source)) == '(') //v kodu mam find(...)
                    {
                        ungetc(c, source);
                        return ID_FUNCTION; //vratim attr = find, token = id_function, obdobne pro ostatni fce i obecne
                    }
                    ungetc(c, source);
                    return FIND;
                }
                else
                if (strCmpConstStr(attr, "FORWARD") == 0) return FORWARD;
                else
                if (strCmpConstStr(attr, "FUNCTION") == 0) return FUNCTION;
                else
                if (strCmpConstStr(attr, "IF") == 0) return IF;
                else
                if (strCmpConstStr(attr, "INTEGER") == 0) return T_INTEGER;
                else
                if (strCmpConstStr(attr, "READLN") == 0) return READLN;
                else
                if (strCmpConstStr(attr, "REAL") == 0) return T_REAL;
                else
                if (strCmpConstStr(attr, "SORT") == 0)
                {
                    if((c=getc(source)) == '(')
                    {
                        ungetc(c, source);
                        return ID_FUNCTION;
                    }
                    ungetc(c, source);
                    return SORT;
                }
                else
                if (strCmpConstStr(attr, "STRING") == 0) return T_STRING;
                else
                if (strCmpConstStr(attr, "THEN") == 0) return THEN;
                else
                if (strCmpConstStr(attr, "TRUE") == 0) return BOOLEAN;
                else
                if (strCmpConstStr(attr, "VAR") == 0) return VAR;
                else
                if (strCmpConstStr(attr, "WHILE") == 0) return WHILE;
                else
                if (strCmpConstStr(attr, "WRITE") == 0) return WRITE;
                else
                if (strCmpConstStr(attr, "COPY") == 0)
                {
                    if((c=getc(source)) == '(')
                    {
                        ungetc(c, source);
                        return ID_FUNCTION;
                    }
                    ungetc(c, source);
                    return COPY;
                }
                else
                if (strCmpConstStr(attr, "LENGTH") == 0)
                {
                    if((c=getc(source)) == '(')
                    {
                        ungetc(c, source);
                        return ID_FUNCTION;
                    }
                    ungetc(c, source);
                    return LENGTH;
                }
                else
                {
                    if((c=getc(source)) == '(')
                    {
                        ungetc(c, source);
                        return ID_FUNCTION;
                    }
                    ungetc(c, source);
                    return ID;
                }
              }
           break;

           case 4:
            //mensi
            if ((c == '_')||(isalnum(c)) || (c == ' '))
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
            if((c == '_')||(isalnum(c))||(c == ' '))
            {
                ungetc(c, source);
                return L;//vetsitko
            }
            else
                if(c == '=') return LE;//vetsi nebo rovno
                    else return LEX_ERROR;
            break;

           case 6:  //int nebo double
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                }
                else
                {
                    if(c == '.')    //desetinne cislo
                    {
                        strAddChar(attr, c);
                        obs = 0;
                        state = 7;
                    }
                    else
                    {
                        if(c == 'e' || c == 'E')    //cislo s exponentem
                        {
                            strAddChar(attr, c);
                            obs = 0;
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

           case 7:  //desetinne cislo
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                    obs = 1;    //desetinna cast musi neco obsahovat
                }
                else
                {
                    if(obs == 0) return LEX_ERROR;  //ukonceni desetinne casti bez obsahu
                    if(c == 'e' || c == 'E')    //desetinne cislo s exp
                    {
                        strAddChar(attr, c);
                        obs = 0;
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

           case 8:  //desetinne cislo s exp
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
               else
               {
                   if(c == '-') //zaporny exp
                   {
                        obs = 0;
                        strAddChar(attr, c);
                        state = 9;
                   }
                   else
                   {
                       if(c == '+')
                        {
                            if (plusko == PRAVDA) return LEX_ERROR;
                          state = 8;  //kladny exp nema vliv
                          plusko = PRAVDA;
                        }
                       else
                       {
                           if(isalpha(c) && ((c != ' ') || (c != ';'))) return LEX_ERROR;
                            else
                            {
                                if(obs == 0) return LEX_ERROR;
                                ungetc(c, source);
                                obs = 0;
                                return DES_EXP;
                            }
                       }
                   }
               }
            break;

           case 9:  //desetinne cislo se zapornym exp
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
                else
                {
                    if(obs == 0) return LEX_ERROR;
                    if(isalpha(c) && ((c != ' ') || (c != ';'))) return LEX_ERROR;
                    else
                    {
                        ungetc(c, source);
                        obs = 0;
                        return DES_EXP_NEG;
                    }
                }
            break;

           case 10: //pouze X.e^(neco)

               if(c>='0' && c<='9')
                {
                    obs = 1;
                    strAddChar(attr, c);
                }
               else
               {
                   if(c == '-') //e^-neco
                   {
                        strAddChar(attr, c);
                        obs = 0;
                        state = 11;
                   }
                   else
                   {
                       if(c == '+')
                       {
                           //printf("\n\ntady u pluska\n");
                           if(plusko == PRAVDA) return LEX_ERROR;
                           else plusko = PRAVDA;

                            obs = 0;
                           state = 10; //e^+neco
                       }
                       else
                       {
                           if(isalpha(c) && ((c != ' ') || (c != ';'))) return LEX_ERROR;
                            else
                            {
                                if(obs == 0) return LEX_ERROR;
                                ungetc(c, source);
                                obs = 0;
                                return EXP;
                            }
                       }
                   }
               }
            break;

           case 11: //e^-neco
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
                else
                {
                    if(obs == 0) return LEX_ERROR;
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
               if(c == '=') return ASS; //prirazeni :D
               else if(isalnum(c) || c == ' ')  //" : neco "
               {
                    ungetc(c, source);
                   return COLON;
               }
               else return LEX_ERROR;
            break;

           case 13:
               /* rozsirit o povolene/nepovolene znaky  */
               if(c == APS)
               {
                   state = 14;
                }
               else if(c == '\n')  //APS = 39 = ascii hodnota apostrofu
               {
                   return LEX_ERROR;
               }
               else
               {
                   if(c >= 31 && c<=255) strAddChar(attr, c); /*ascii znaky mezi 31, 255 vcetne se zapisuji primo, krom APS,
                   ostatni musi podle escape sekvence, podle zadani*/
                   else return LEX_ERROR;
               }
            break;

           case 14:
               if(c == APS) //dva apostrofy za sebou
               {
                   //printf("\n  APOSTROF nacitam: %c\n", APS);
                   strAddChar(attr, APS);
                   state = 13;
               }
               else if(c == '#') //escape sekvence
               {
                   obs = 0;
                   state = 15;
               }
               else
               {
                   if(!(isalnum(c)) || (c == ' '))
                   {
                       if(obs == -1) return LEX_ERROR;
                       //printf("vracim retezec \n");
                        ungetc(c, source);
                        return STRING;
                   }
                   else
                    return LEX_ERROR;

               }
            break;

           case 15: //escape sekvence
               if(c>='0' && c<= '9')    //obsahuje nejake cisla, ty ulozim do pom a potom je vypisu jako jeden znak
                {
                    if(obs == 0 )
                    {
                        if(c != '0')
                        {
                            //printf("lepim... %c\n", c);
                            strAddChar(&pom, c);
                            obs = 1;
                        }
                        else
                        {
                            obs = -1;
                        }
                    }
                    else
                    {
                        strAddChar(&pom, c);
                        obs = 1;
                    }
                }
                else if(c == APS)
                {
                    if (obs == 0) return LEX_ERROR;
                    //printf("prevadim: %d\n", atoi((&pom)->str));
                    int esc = atoi((&pom)->str);
                    if (esc < 1 || esc > 255) return LEX_ERROR;
                    strAddChar(attr, esc);
                    strClear(&pom);
                    obs = 0;
                    state = 13;
                }
                else
                {
                    return LEX_ERROR;
                }
            break;
        }
      }
    }
