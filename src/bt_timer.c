/*
 * The user space's multi-timer support based on setitimer()
 *
 */
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "bt_timer.h"
#include "bttrace.h"

#ifdef _UNIT_TEST
extern void mock_assert(const int result, const char* const expression,
		                        const char * const file, const int line);
#undef assert
#define assert(expression) \
	mock_assert((int)(expression), #expression, __FILE__, __LINE__);

extern void* _test_malloc(const size_t size, const char* file, const int line);
extern void* _test_calloc(const size_t number_of_elements, const size_t size,
		                          const char* file, const int line);
extern void _test_free(void* const ptr, const char* file, const int line);

#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)
#endif /* _UNIT_TEST */

static struct bt_timer_list timer_list;
static struct list_head *get_position(int interval, struct list_head *list);
static void bt_timer_func(int signo);

static struct list_head *get_position(int interval, struct list_head *list)
{
	struct bt_timer *vec;

	list_for_each_entry(vec, list, head)
		if (vec->interval > interval)
			return &vec->head;
	return &vec->head;
}

static void bt_timer_func(int signo)
{
	struct bt_timer *vec, *tmp;

	list_for_each_entry_safe(vec, tmp, &timer_list.list, head) {
		vec->elapse ++;
		if (vec->elapse >= vec->interval) {
			vec->elapse = 0;
			vec->callback(vec->timer_id, vec->data);
			list_del(&vec->head);
			if (vec->type != BT_TIMER_SINGLE) {
				list_add_tail(&vec->head, get_position(vec->interval, &timer_list.list));
			}
		}
	}
}

int bt_timer_init(void)
{
	memset(&timer_list, 0, sizeof(timer_list));
	INIT_LIST_HEAD(&timer_list.list);

	if (SIG_ERR == (timer_list.old_sigfunc = signal(SIGALRM, bt_timer_func))) {
		return -1;
	}
	timer_list.new_sigfunc = bt_timer_func;
	timer_list.value.it_value.tv_sec = 0; 
	timer_list.value.it_value.tv_usec = 1; 
	timer_list.value.it_interval.tv_sec = 1;
	timer_list.value.it_interval.tv_usec = 0;
	return setitimer(ITIMER_REAL, &timer_list.value, &timer_list.ovalue);
}

/* func:	run when time up
 * data:	func paramater
 * interval:	timer expire time
 * type:	BT_TIMER_SINGLE or BT_TIMER_REPEAT : run the function only once or looping.
 *
 * return:	the timer_id, should be used when delete timer.
 */
struct bt_timer *bt_timer_add(timer_expired func, void *data, unsigned int interval, int type)
{
	struct bt_timer	*ptr = malloc(sizeof(struct bt_timer));

	memset(ptr, 0, sizeof(struct bt_timer));
	ptr->timer_id = timer_list.num ++;
	ptr->callback = func;
	ptr->data = data;
	ptr->type = type;
	ptr->elapse = 0;
	ptr->interval = interval;

	list_add_tail(&ptr->head, get_position(interval, &timer_list.list));
	return ptr;
}

void bt_timer_del(struct bt_timer *ptr)
{
	list_del(&ptr->head);
	free(ptr);
}

void bt_timer_update(struct bt_timer *ptr)
{
	list_del(&ptr->head);
	ptr->elapse = 0;
	list_add_tail(&ptr->head, get_position(ptr->interval, &timer_list.list));
}

void bt_timer_finit(void)
{
	struct bt_timer *vec, *tmp;

	signal(SIGALRM, timer_list.old_sigfunc);
	list_for_each_entry_safe(vec, tmp, &timer_list.list, head)
		bt_timer_del(vec);
	memset(&timer_list, 0, sizeof(timer_list));
}

#undef _UNIT_TEST
#ifdef _UNIT_TEST
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include "unittest/cmockery.h"

void test_func(int timer_id, void *data)
{
	TRACE(INFO, "timer_id %d\n", timer_id);
}

int main(int argc, char **argv)
{
	TRACE(INFO, "BT Timer Test.\n");
	bt_timer_init();
	bt_timer_add(test_func, NULL, 5, BT_TIMER_SINGLE);
	bt_timer_add(test_func, NULL, 1, BT_TIMER_SINGLE);
	bt_timer_add(test_func, NULL, 2, BT_TIMER_REPEAT);

	while (1) {
		usleep(1);
	}
	return 0;
}

#endif /* _UNIT_TEST */
