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
