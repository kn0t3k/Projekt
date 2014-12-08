#include <stdlib.h>
#include "garbage.h"

struct garbage_list* garbage_init(){
	struct garbage_list* new = malloc(sizeof(struct garbage_list));
	if(new == NULL) return NULL;
	
	new->first = NULL;
	
	return new;
}


int garbage_add(struct garbage_list* Glist, void* Gptr){
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
