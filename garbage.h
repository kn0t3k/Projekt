
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
