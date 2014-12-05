/*--------------------------------------
| Projekt: IFJ14						
| Soubor: scaner.h						
| Autori: Denk Filip (xdenkf00)		
|	  Jurica Tomas (xjuric22)		
|	  Knotek Martin (xknote11)	
|	  Kohut Jan (xkohut08)		
|	  Piskula David (xpisku02)	
| Prekladac: GCC 4.8.2-7				
| Datum:   5.12.2014					
|--------------------------------------*/

//hlavicka funkce simulujici lexikalni analyzator
void setSourceFile(FILE *f);
void prilep(int z, int y);
int getNextToken(string *attr);
int printToken(string *attr);
