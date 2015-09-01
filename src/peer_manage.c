#include "peer_manage.h"

//TODO: should pass the task struct pointer into peer_alloc()
struct peer_mgnt *peer_alloc(struct b_string *peer_id)
{
	struct peer_mgnt *ptr = malloc(sizeof(struct peer_mgnt));
	ptr->peer_id = peer_id;

	return ptr;
}

void peer_free(struct peer_mgnt *ptr)
{
	list_del(&ptr->head);
	b_string_free(ptr->peer_id);
	free(ptr);
}

