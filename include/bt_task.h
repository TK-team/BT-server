#ifndef _BT_TASK_H_
#define _BT_TASK_H_

#include "b_parse.h"
#include "peer_manage.h"
#include "torrent.h"

struct bt_task {
	struct torrent *torrent_info;
	struct list_head peer_list;
};

#endif /* _BT_TASK_H_ */
