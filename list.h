/*--------------------------------------
| Projekt: IFJ14						
| Soubor: list.h						
| Autori: Denk Filip (xdenkf00)		
|	  Jurica Tomas (xjuric22)		
|	  Knotek Martin (xknote11)	
|	  Kohut Jan (xkohut08)		
|	  Piskula David (xpisku02)	
| Prekladac: GCC 4.8.2-7				
| Datum:   5.12.2014					
|--------------------------------------*/

typedef struct
{
  int Type;  //typ instrukce
  void *addr1; //adresa 1
  void *addr2; //adresa 2
  void *addr3; //adresa 3
} tInstr;

typedef struct Item
{
  tInstr Instruction;
  struct Item *NextItem;
} tItem;
    
typedef struct
{
  struct Item *First;  //ukazatel na prvni prvek
  struct Item *Last;   //ukazatel na posledni prvek
  struct Item *Act; //ukazatel na aktivni prvek
} tList;

void InitList (tList *L);
void DisposeList (tList *L);
void InsertNew (tList *L, tInstr NewIns);
void *LastItemAddress (tList *L);
void *CurrentItemAddress (tList *L);
void GoToItem (tList *L, void *ItemAddress);
void First (tList *L);
void Last (tList *L);
void NextIns (tList *L);
tInstr *GetData (tList *L);
