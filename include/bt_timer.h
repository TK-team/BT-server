#ifndef _BT_TIMER_H_
#define _BT_TIMER_H_

#include <sys/time.h>

#include "list.h"

enum BT_TIMER_TYPE{
	BT_TIMER_SINGLE = 0,
	BT_TIMER_REPEAT = 1,
};

typedef void (*timer_expired)(int timer_id, void *data);

struct bt_timer {
	struct list_head head;
	unsigned timer_id;
	timer_expired callback;
	void *data;
	int type;
	unsigned int interval;
	unsigned int elapse;
};

struct bt_timer_list {
	struct list_head list;
	struct itimerval value;
	struct itimerval ovalue;
	void (*old_sigfunc)(int);
	void (*new_sigfunc)(int);
	int num;
};

static struct bt_timer_list timer_list;
static struct list_head *get_position(int interval, struct list_head *list);
static void bt_timer_func(int signo);

extern int bt_timer_init(void);
extern int bt_timer_add(timer_expired func, void *data, int interval, int type);
extern void bt_timer_del(int timer_id);
extern void bt_timer_finit(void);

#endif /* _BT_TIMER_H_ */
