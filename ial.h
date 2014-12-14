/*--------------------------------------
| Projekt: IFJ14						
| Soubor: ial.h						
| Autori: Denk Filip (xdenkf00)		
|	  Jurica Tomas (xjuric22)		
|	  Knotek Martin (xknote11)	
|	  Kohut Jan (xkohut08)		
|	  Piskula David (xpisku02)	
| Prekladac: GCC 4.8.2-7				
| Datum:   5.12.2014					
|--------------------------------------*/

#include <stdbool.h>
#include <limits.h>


//----------------------------------- HLAVICKOVY SOUBOR ial.h ------------------------------------


#define SIZE 487   //velikost hashovaci tabulky, idealne prvociselna
/*		
*		S touto velikosti tabulky bylo dosahnuto nejlepsiho a nejuniverzalnejsiho pomeru spotrebovana pamet/spotrebovany cas
*		a to i pro vzorky s mensim nebo vetsim poctem jedinecnych slov. (Testovano na vzorku, ktery obsahoval 2 995 362 slov, z toho priblizne 87 000 jedinecnych.)
*		S klesajicim poctem jedinecnych slov v textu klesa pametova efektivita, s roztoucim poctem naopak casova.
*		Zvolena prvociselna hodnota pro lepsi rozptyleni vysledku hash funkce. (Zdroj http://en.wikipedia.org/wiki/Hash_table#Choosing_a_good_hash_function
*
*/

//enum typu pro tabulku, to "s_" je tam, abych se vyhnul kolizim (napr. string ve str.h)
enum s_table_type {s_integer, s_real, s_string, s_boolean, s_default};


typedef struct symbol_table{
	struct symbol_table_item* global;
	struct symbol_table_item* local;
	} symbol_table;
	
typedef struct symbol_table_item{
	struct htab_t* table;
	unsigned int item_count;
 	struct symbol_table_item* next;
	} symbol_table_item;

typedef struct htab_item{ 
	char *name;
	enum s_table_type type;    	
	char* func_data;
	bool global;
	bool function;
	bool initialized;
	bool fwd;
	int index;
	void* label;
	struct symbol_table_item* func_table; 	
	struct htab_item* next;
	} htab_item;


typedef struct htab_t{ 
	unsigned int htab_size;
	htab_item* ptr[]; 
	} htab_t;



unsigned int hash_function(const char *str, unsigned htab_size); //bude v samostatnem modulu pro jednodussi zmenu hashovaci fce
htab_t* htab_init(unsigned int htab_size);  //inicializace tabulky na pozadovanou velikost


struct symbol_table* symbol_table_init();
void add_local_table(struct symbol_table* s_table, int* error);
void remove_local_table(struct symbol_table* s_table, int* error);
struct htab_item* add_var(char *name_notupper, struct symbol_table* s_table, int* error);
struct htab_item* add_func(char *name_notupper, struct symbol_table* s_table, int* error);
struct htab_item* search_func(char *name_notupper, struct symbol_table* s_table, int* error);
struct htab_item* search_var(char *name_notupper, struct symbol_table* s_table, int* error);
void symbol_table_free(struct symbol_table* s_table);
int funcs_defined(struct symbol_table* s_table); //overi, zda vsechny deklarovane fce v tabulce byly i definovane. pokud ne -> return SEM_ERROR

int length(string *s);
int copy(char *s, string *dest, int i, int n, int s_len);
int find(char *s, char *search, int s_len, int search_len);
void sort(string *dest);
void sort_main(char *A, int left, int right);
void partition(char *A, int left, int right, int *i, int *j);

int readln_int(int *);
double readln_real(int *);
char* readln_string(int *);
char* doubleAddChar(char *, char, int *, int *);
