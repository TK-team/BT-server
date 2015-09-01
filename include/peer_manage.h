#ifndef _PEER_MANAGE_H_
#define _PEER_MANAGE_H_

#include "list.h"
#include "b_parse.h"

#define PEER_ID_LEN			20

struct peer_state {
	int client_stat;
	int peer_stat;
};

struct peer_stat {
	unsigned int upload;
	unsigned int download;
	unsigned int upload_speed;
	unsigned int download_speed;
};

struct peer_mgnt {
	struct list_head head;
	unsigned int ip;
	unsigned short port;
	int sock;
	struct b_string *peer_id;
	struct bt_task *task;
	struct peer_state state;
	//TODO: Send buffer and Receive buffer.
	//TODO: Data request list
	//TODO: Data be requested list
	struct peer_stat stat;
	//TODO: Timer for keep alive
};

#endif /* _PEER_MANAGE_H_ */
