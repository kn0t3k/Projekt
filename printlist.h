/*--------------------------------------
| Projekt: IFJ14
| Soubor: printlist.h
| Autori: Denk Filip (xdenkf00)
|	  Jurica Tomas (xjuric22)
|	  Knotek Martin (xknote11)
|	  Kohut Jan (xkohut08)
|	  Piskula David (xpisku02)
| Prekladac: GCC 4.8.2-7
| Datum:   5.12.2014
|--------------------------------------*/

typedef struct PrintItem
{
	int type;
	void *var;
	char *str;
	struct PrintItem *NextItem;
} tPrintItem;
    
typedef struct
{
	struct PrintItem *First;  //ukazatel na prvni prvek
	struct PrintItem *Last;   //ukazatel na posledni prvek
	struct PrintItem *Act; //ukazatel na aktivni prvek
} tPrintList;

void InitPrintList (tPrintList *L);
int InsertPrintNew (tPrintList *L, int type, void *var);
void PrintAll (tPrintList *L);
