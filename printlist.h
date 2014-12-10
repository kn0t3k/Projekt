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
