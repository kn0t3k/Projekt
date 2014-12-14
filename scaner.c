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

    #include <stdio.h>
    #include <ctype.h>
    #include <stdlib.h>
    #include "err.h"
    #include "str.h"
    #include "scaner.h"

    FILE *source;
    short obs = 0;   //kontorla, zda cisla exponentu, nebo des. casti obsahuji nejakou hodnotu
    string pom;	//pro ukladani a kontrolu escape sekvenci


    void setSourceFile(FILE *f)
    {
      source = f;
    }

    int getNextToken(string *attr)//tuto funkci vola praser a ona vola maketoken aby mohla vytisknout tokeny
    {
        strInit(&pom);
        int vysledek = makeToken(attr);
		
		/*
		fce slouzila k vypisovani tokenu a slouzi k uvolneni mista pro promennou
		*/

        strFree(&pom);
        return vysledek;
    }


    int makeToken(string *attr)
    // hlavni funkce scaneru
    {
       int state = 0;
       int c;
       int plusko = NEPRAVDA;

       strClear(attr);
       strClear(&pom);

       while (1)
       {
         c = getc(source);

         switch (state)
         {
           case 0:
             if (isspace(c)) state = 0;
             else
             if (c == '{') state = 1;  //komentar
             else
             if(c>='0' && c<='9') //jedna se o cislo - int/double
             {
                 strAddChar(attr, c);
                 state = 5;
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
             if (c == ':') state = 11;
             else
             if (c == EOF) return END_OF_FILE;
             else
             if (c == '>') state = 4;
             else
             if (c == '<') state = 3;
             else
             if(c == APS) state = 12;   //APS = 39 = ascii hodnota apostrofu
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
                        return ID_FUNCTION; //vratim attr = find, token = id_function, obdobne pro ostatni vestavene fce i  uzivatelem definovane
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

           case 3:
            //mensi
                if(c == '=') return SE;//mensi nebo rovno
                else
                if(c == '>') return SL;//nerovno
                else	//vratim posledni znak a mensitko
                {
                    ungetc(c, source);
                    return S;
                }
            break;

           case 4:
               //vetsi
                if(c == '=') return LE;
                else
                {
                    ungetc(c, source);
                    return L;
                }
            break;

           case 5:  //int nebo double
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
                        state = 6;
                    }
                    else
                    {
                        if(c == 'e' || c == 'E')    //cislo s exponentem
                        {
                            strAddChar(attr, c);
                            obs = 0;
                            state = 9;
                        }
                        else
                        {
                            if(isalpha(c)) return LEX_ERROR;
                            else
                            {
                                ungetc(c, source);
                                return INTEGER;
                            }
                        }
                    }
                }
            break;

           case 6:  //desetinne cislo
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
                        state = 7;
                    }
                    else
                    {
                        if(isalpha(c)) return LEX_ERROR;	//pokud mezi cisly je pismeno
                        else
                        {
                            ungetc(c, source);
                            obs = 0;
                            return DES_INT;
                        }
                    }
                }
            break;

           case 7:  //desetinne cislo s exp
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
               else
               {
                   if(c == '-') //zaporny exp
                   {
                    /*
                        nacita cisla do exponentu
                        pokud nacte '-', zkontroluje, jestli uz exponent ma nejakou hodnotu,
                        pokud ano, musi se jednat o minus, ktere patri mezi dva operandy, proot vrati token;
                        pokud ne, je to minus, ktere patri k zapornemu exponentu
                    */
                       if(obs == 1)
                       {
                           ungetc(c, source);
                           return DES_EXP;
                       }
                       if(plusko == PRAVDA) return LEX_ERROR;//pokud by se vyskytlo 3e+-3
                        obs = 0;
                        strAddChar(attr, c);
                        state = 8;
                   }
                   else
                   {
                       if(c == '+')
                        {
                            /*
                            viz vyse, analogicky
                            */
                            if(obs == 1)
                           {
                               ungetc(c, source);
                               return DES_EXP;
                           }
                            if(plusko == PRAVDA) return LEX_ERROR;
                            else plusko = PRAVDA;//doslo k nacteni plus v exponentu
                            obs = 0;
                            state = 8;
                        }
                       else
                       {
                           if( c == '*')
                           {
                               if(obs == 1)
                               {
                                   ungetc(c, source);
                                   return DES_EXP;
                               }
                               else return LEX_ERROR;
                           }
                           if( c == '/')
                           {
                               if(obs == 1)
                               {
                                   ungetc(c, source);
                                   return DES_EXP;
                               }
                               else return LEX_ERROR;
                           }
                           if(isalpha(c)) return LEX_ERROR;
                            else
                            {
                                if(obs == 0) return LEX_ERROR;//kontrola obsaha exponentu
                                ungetc(c, source);
                                obs = 0;
                                plusko = NEPRAVDA;
                                return DES_EXP;
                            }
                       }
                   }
               }
            break;

           case 8:  //desetinne cislo se zapornym exp
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
                else
                {
                    if(obs == 0) return LEX_ERROR;
                    if(isalpha(c)) return LEX_ERROR;
                    else
                    {
                        ungetc(c, source);
                        obs = 0;
                        return DES_EXP_NEG;
                    }
                }
            break;

           case 9: //pouze X.e^(neco)
               if(c>='0' && c<='9')
                {
                    obs = 1;
                    strAddChar(attr, c);
                }
               else
               {
                   if(c == '-') //e^-neco
                   {
                       /*
                       viz case: 8
                       */
                       if(obs == 1)
                       {
                           ungetc(c, source);
                           return EXP;
                       }
                       if(plusko == PRAVDA) return LEX_ERROR;
                        strAddChar(attr, c);
                        obs = 0;
                        state = 10;
                   }
                   else
                   {
                       if(c == '+')
                       {
                           /*
                           viz case: 8
                           */
                           if(obs == 1)
                           {
                               ungetc(c, source);
                               return EXP;
                           }
                           if(plusko == PRAVDA) return LEX_ERROR;
                           else plusko = PRAVDA;
                            obs = 0;
                           state = 9; //e^+neco
                       }
                       else
                       {
                           if( c == '*')
                           {
                               if(obs == 1)
                               {
                                   ungetc(c, source);
                                   return EXP;
                               }
                               else return LEX_ERROR;
                           }
                           if( c == '/')
                           {
                               if(obs == 1)
                               {
                                   ungetc(c, source);
                                   return EXP;
                               }
                               else return LEX_ERROR;
                           }
                           if(isalpha(c)) return LEX_ERROR;
                            else
                            {
                                if(obs == 0) return LEX_ERROR;
                                ungetc(c, source);
                                obs = 0;
                                plusko = NEPRAVDA;
                                return EXP;
                            }
                       }
                   }
               }
            break;

           case 10: //e^-neco
               if(c>='0' && c<='9')
                {
                    strAddChar(attr, c);
                    obs = 1;
                }
                else
                {
                    if(obs == 0) return LEX_ERROR;
                    if(isalpha(c)) return LEX_ERROR;
                    else
                    {
                        ungetc(c, source);
                        obs = 0;
                        return EXP_NEG;
                    }
                }
            break;

           case 11:
               if(c == '=') return ASS; //prirazeni
               else
               {
                    ungetc(c, source);
                   return COLON;
               }
            break;

           case 12: //prijima string
               if(c == APS)//APS = 39 = ascii hodnota apostrofu
               {
                   state = 13;
               }
               else
               {
                   if(c >= 31 && c<=255) strAddChar(attr, c); 
				   /*
				   ascii znaky <31, 255> se zapisuji primo, krom APS,
                   ostatni musi podle escape sekvence, podle zadani
				   */
                   else return LEX_ERROR;
               }
            break;

           case 13:
               if(c == APS) //dva apostrofy za sebou
               {
                   strAddChar(attr, APS);	//zapiseme APS do vysledneho stringu
                   state = 12;
               }
               else if(c == '#') //escape sekvence
               {
                   obs = 0;
                   state = 14;
               }
               else
               {
                   if(obs == -1) return LEX_ERROR;	//v esc sekvenci jsou same nuly
                        ungetc(c, source);
                        return STRING;
               }
            break;

           case 14: //escape sekvence
               if(c>='0' && c<= '9')    //obsahuje nejake cisla, ty ulozim do pom a potom je vypisu jako jeden znak
                {
                    if(obs == 0 )
                    {
                        if(c != '0')
                        {
                            strAddChar(&pom, c);
                            obs = 1;
                        }
                        else
                        {
                            obs = -1;	
							/*
							pokud v pom je aktualne nula a pokud je nactena dalsi nula tak obs = -1, pokud tento stav skonci
							s obs == -1 tak byly zjisteny same nuly, zajistuje ignorovani pocatecnich nul
							pokud esc sekvence uz neco obsahuje (obs == 1), tak se vsechna dalsi cisla pripojuji
							*/
                        }
                    }
                    else
                    {
                        strAddChar(&pom, c);
                        obs = 1;
                    }
                }
                else if(c == APS)	// ukonceni esc sekvence
                {
                    if (obs == 0) return LEX_ERROR;//pokud esc sekvence nic neobsahuje
                    int esc = atoi((&pom)->str);
                    if (esc < 1 || esc > 255) return LEX_ERROR;//pokud obsah sekvenec neodpovida zadani
                    strAddChar(attr, esc);	//cisla esc sekv. vyjadrujici znak pripojime jako znak do vysledneho stringu
                    strClear(&pom);
                    obs = 0;
                    state = 12;
                }
                else return LEX_ERROR;
            break;

            default: return LEX_ERROR;//neznamy znak na vstupu
        }
      }
    }
