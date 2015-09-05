#ifndef _BT_TASK_H_
#define _BT_TASK_H_

#include "b_parse.h"
#include "peer_manage.h"
#include "torrent.h"

struct bt_task {
	struct torrent *torrent_info;
	struct list_head unuse_peer_list;
	struct list_head inuse_peer_list;
	unsigned short listen_port;
	struct b_string *info_hash;
	struct b_string *peer_id;
	unsigned long long downloaded;
	unsigned long long left;
	unsigned int compact;
	int event;
	int max_peer_num;
	/* optional begin */
	int ip;
	unsigned int numwant;
	int key;
	int trackerid;
	/* optional end */
};

#endif /* _BT_TASK_H_ */
