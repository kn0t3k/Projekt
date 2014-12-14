/*--------------------------------------
| Projekt: IFJ14
| Soubor: garbage.c
| Autori: Denk Filip (xdenkf00)
|	  Jurica Tomas (xjuric22)
|	  Knotek Martin (xknote11)
|	  Kohut Jan (xkohut08)
|	  Piskula David (xpisku02)
| Prekladac: GCC 4.8.2-7
| Datum:   5.12.2014
|--------------------------------------*/

#include <stdlib.h>
#include "garbage.h"

struct garbage_list* garbage_init(){
	struct garbage_list* new = malloc(sizeof(struct garbage_list));
	if(new == NULL) return NULL;
	
	new->first = NULL;
	
	return new;
}


int garbage_add(struct garbage_list* Glist, void* Gptr){
	struct garbage_item* tmp = Glist->first;
	while(tmp){
		if(tmp->ptr == Gptr) return 0;
		tmp = tmp->next;
	}
	
	
	struct garbage_item* new = malloc(sizeof(struct garbage_item));
	if(new == NULL) return 1;
	
	new->ptr = Gptr;
	
	new->next = Glist->first;
	Glist->first = new;
	
	return 0;
}

void garbage_free(struct garbage_list* Glist){
	struct garbage_item* delete = NULL;
	
	while(Glist->first){
		delete = Glist->first;
		Glist->first = Glist->first->next;
		free(delete->ptr);
		free(delete);
	}
	free(Glist);
}
