/*--------------------------------------
| Projekt: IFJ14
| Soubor: garbage.h
| Autori: Denk Filip (xdenkf00)
|	  Jurica Tomas (xjuric22)
|	  Knotek Martin (xknote11)
|	  Kohut Jan (xkohut08)
|	  Piskula David (xpisku02)
| Prekladac: GCC 4.8.2-7
| Datum:   5.12.2014
|--------------------------------------*/

typedef struct garbage_item{
	void* ptr;
	struct garbage_item* next;
	} garbage_item;

typedef struct garbage_list{
	struct garbage_item* first;
	} garbage_list;	
	
struct garbage_list* garbage_init();	
int garbage_add(struct garbage_list* Glist, void* Gptr);
void garbage_free(struct garbage_list* Glist);
