#ifndef _PEER_MANAGE_H_
#define _PEER_MANAGE_H_

#include "list.h"
#include "b_parse.h"
#include "bt_task.h"

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
	//Use to maintain the keep_alive message receive from peer per 120s.
	struct bt_timer *rcv_keep_alive;
	//Use to maintain the keep_alive message send to peer per 120s.
	struct bt_timer *send_keep_alive;
};

#endif /* _PEER_MANAGE_H_ */
