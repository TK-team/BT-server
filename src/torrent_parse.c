#include "torrent.h"

struct torrent *torrent_alloc(void)
{
	struct torrent *ptr = malloc(sizeof(struct torrent));

	if (ptr) {
		return ptr;
	}
	return NULL;
}

int torrent_free(struct torrent *ptr)
{
	return 0;
}

int torrent_parse(char *buf, struct torrent *ptr)
{
	return 0;
}

