/********* ial.h **********
*							            *
* Projekt: IFJ 				    *
* Překladač: GCC 4.8.2-7	*
* Datum:   19.11.2014		  *
* 							          *
***************************/

#include <stdbool.h>

//----------------------------------- HLAVICKOVY SOUBOR ial.h ------------------------------------


#define SIZE 971   //velikost hashovaci tabulky, idealne prvociselna
/*		
*		S touto velikosti tabulky bylo dosahnuto nejlepsiho a nejuniverzalnejsiho pomeru spotrebovana pamet/spotrebovany cas
*		a to i pro vzorky s mensim nebo vetsim poctem jedinecnych slov. (Testovano na vzorku, ktery obsahoval 2 995 362 slov, z toho priblizne 87 000 jedinecnych.)
*		S klesajicim poctem jedinecnych slov v textu klesa pametova efektivita, s roztoucim poctem naopak casova.
*		Zvolena prvociselna hodnota pro lepsi rozptyleni vysledku hash funkce. (Zdroj http://en.wikipedia.org/wiki/Hash_table#Choosing_a_good_hash_function
*
*/
typedef struct symbol_table{
	struct symbol_table_item* global;
	struct symbol_table_item* local;
	} symbol_table;
	
typedef struct symbol_table_item{
	struct htab_t* table;
 	struct symbol_table_item* next;
	} symbol_table_item;



typedef struct htab_item{ 
	char *name;
	enum s_table_type type;    	
	char* func_data;
	bool global;
	bool function;
	bool initialized;
	int index;
	struct symbol_table_item* func_table; 	
	struct htab_item* next;
	} htab_item;

typedef struct htab_t{ 
	unsigned int htab_size;
	unsigned int item_count;
	htab_item* ptr[]; 
	} htab_t;


//pouze pomocne fce, vetsina zmizi
unsigned int hash_function(const char *str, unsigned htab_size); 

htab_t* htab_init(unsigned int htab_size);  //inicializace tabulky na pozadovanou velikost
void htab_free(htab_t* t); //kompletni uvolneni
htab_item* htab_search(htab_t *t,const char *name); //vzhleda prvek podle name, pokud neni vraci NULL
void htab_remove(htab_t *t, const char *name);  //fce odstrani prvek specifikovany argumentem name
htab_item* htab_add(htab_t *t,char *name); //prida prvek, nejspis bude potreba predelat bud pro vice argumentu nebo predavat cely novy prvek pro pridani pres ukazatel


//hlavni fce pro obsluhu tabulky
struct symbol_table* symbol_table_init(int* error);
void add_local_table(struct symbol_table* s_table, int* error);
void remove_local_table(struct symbol_table* s_table, int* error);
struct htab_item* add_var(char *name, struct symbol_table* s_table, int* error);
struct htab_item* add_func(char *name, struct symbol_table* s_table, int* error);
//search
					
