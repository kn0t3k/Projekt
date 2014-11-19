/********* htable.h *********
*							*
* Projekt: IFJ 				*
* Překladač: GCC 4.8.2-7	*
* Datum:   19.11.2014		*
* 							*
****************************/

//----------------------------------- HLAVICKOVY SOUBOR htable.h ------------------------------------


#define SIZE 971   //velikost hashovaci tabulky, idealne prvociselna
/*		
*		S touto velikosti tabulky bylo dosahnuto nejlepsiho a nejuniverzalnejsiho pomeru spotrebovana pamet/spotrebovany cas
*		a to i pro vzorky s mensim nebo vetsim poctem jedinecnych slov. (Testovano na vzorku, ktery obsahoval 2 995 362 slov, z toho priblizne 87 000 jedinecnych.)
*		S klesajicim poctem jedinecnych slov v textu klesa pametova efektivita, s roztoucim poctem naopak casova.
*		Zvolena prvociselna hodnota pro lepsi rozptyleni vysledku hash funkce. (Zdroj http://en.wikipedia.org/wiki/Hash_table#Choosing_a_good_hash_function
*
*/


typedef struct htab_listitem{ 
	char *key;  //doplnit, co bude potreba
	//typ    	//nejaky enum typ, ktery se nejspis includne z jineho souboru
	//hodnota ??
	struct htab_listitem* next;
	} htab_listitem;

typedef struct htab_t{ 
	unsigned int htab_size;
	htab_listitem* ptr[];
	} htab_t;



unsigned int hash_function(const char *str, unsigned htab_size); //bude v samostatnem modulu pro jednodussi zmenu hashovaci fce

htab_t* htab_init(unsigned int htab_size);  //inicializace tabulky na pozadovanou velikost
void htab_free(htab_t* t); //kompletni uvolneni
htab_listitem* htab_search(htab_t *t,const char *key); //vzhleda prvek podle key, pokud neni vraci NULL
void htab_remove(htab_t *t, const char *key);  //fce odstrani prvek specifikovany argumentem key
htab_listitem* htab_add(htab_t *t,char *key); //prida prvek, nejspis bude potreba predelat bud pro vice argumentu nebo predavat cely novy prvek pro pridani pres ukazatel
					

