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
  struct Item *First;  //ukazatel na prvni instrukci programu
  struct Item *Last;   //ukazatel na posledni prvek
  struct Item *Act; //ukazatel na aktivni prvek
  struct Item *Top; //ukazatel na prvni prvek seznamu
} tList;

void InitList (tList *LS);
void SetFirst(tList *LS, void *ItemAddress);
void DisposeList (tList *LS);
int InsertNew (tList *LS, tInstr NewIns);
void *LastItemAddress (tList *LS);
void *CurrentItemAddress (tList *LS);
void GoToItem (tList *LS, void *ItemAddress);
void First (tList *LS);
void Last (tList *LS);
void NextIns (tList *LS);
tInstr *GetData (tList *LS);
