#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>
#include <event2/event.h>
#include "bt_socket.h"


static struct send_unit *get_send_unit(struct sockaddr_in *dest,
		struct sockaddr_in *source, char *buf, int buflen,
		callback cb, void *arg, int timeout, int retry);

static int get_fd_by_port(unsigned short port)
{
	return 0;
}

/*	Description:	set fd non-block mode
 *	param:		fd
 */
static void set_nonblock(int fd)
{
	int status = fcntl(fd, F_GETFD);

	fcntl(fd, F_SETFD, status | O_NONBLOCK);
}

/*	Description:	set fd block mode
 *	param:		fd
 */
static void set_block(int fd)
{
	int status = fcntl(fd, F_GETFD);

	fcntl(fd, F_SETFD, status ^ O_NONBLOCK);
}

/*	Description:	Init socket
 *	param:		source - local host address info;
 *	param:		flag - TCP(0) or UDP(1);
 *	retry:		retry or not;
 *	return:		fd of socket;
 */
static int init_socket(struct sockaddr_in *source, int flag, int retry)
{
	int fd = 0;
	int ret = 0;

SOCKET_RETRY:
	if (flag == BT_IPPROTO_TCP) {
		fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	else if (flag == BT_IPPROTO_UDP) {
		fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
	else {
		fprintf(stderr, "%s flag(%d) doesn't support\n", __func__, flag);
		goto ERR_DEAL;
	}

	if (fd == -1) {
		fprintf(stderr, "%s socket error(%d)\n", __func__, errno);
		if (retry && (errno == ENOBUFS)) {
			goto SOCKET_RETRY; 
		}
		goto ERR_DEAL;
	}

BIND_RETRY:
	ret = bind(fd, (struct sockaddr *)(source), sizeof(struct sockaddr));
	if (ret == -1) {
		fprintf(stderr, "%s bind error(%d)\n", __func__, errno);
		switch (errno) {
		case EADDRINUSE:
			if (retry) {
				source->sin_port += ntohs(source->sin_port) + 5;
				source->sin_port = htons(source->sin_port);
				goto BIND_RETRY;
			}
			else {
				goto ERR_DEAL;
			}
		case ENOBUFS:
			goto BIND_RETRY;
		default:
			goto ERR_DEAL; 
		}
	}
	set_nonblock(fd);
	return fd;
ERR_DEAL:
	return -1;
}

int sendall(int fd, char *buf, int *len)
{
	int total = 0;
	int bytesleft = *len;
	int n;

	while (total < *len) {
		n = send(fd, buf + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft -= n;
	}
	*len = total;
	return (n == -1) ? -1 : 0;
}

int bt_sync_send(struct sockaddr_in *dest, struct sockaddr_in *source, char *buf,
		int buflen, int timeout, int flag, char **recv)
{
	int fd = 0; 
	int ret = 0;
	int retry = 0;

	//get_send_unit(dest, source, );
	if ((fd == get_fd_by_port(ntohs(source->sin_port))) == 0) {
		fd = init_socket(source, flag, 1);
		if (fd == -1) {
			goto ERR_DEAL;
		}
	}
	if (flag == BT_IPPROTO_TCP) {
CONNECT_RETRY:
		ret = connect(fd, (struct sockaddr *)dest, sizeof(struct sockaddr));
		if (ret == -1) {
			switch (errno) {
			case EADDRINUSE:
			case EINPROGRESS:
			case EISCONN:
				break;
			case EINTR:
			case ETIMEDOUT:
				if (retry++ > 5)
					goto ERR_DEAL;
				else
					goto CONNECT_RETRY;
			default:
				goto ERR_DEAL;
			}
		}

		ret = send(fd, buf, buflen,MSG_DONTWAIT);
		if (ret == -1) {
			switch (errno) {
			case EWOULDBLOCK:
				break;
			case EINTR:
				break;
			}
		}
		if (ret != buflen) {
			;
		}
		
	}
	else if (flag == BT_IPPROTO_UDP) {
		ret = sendall(fd, buf, &buflen);
		if (ret == -1) {
		}
	}
	else {
		fprintf(stderr, "%s flag(%d) doesn't support\n", __func__, flag);
	}

	return 0;
ERR_DEAL:
	fprintf(stderr, "%s send error(%d)\n", __func__, errno);
	return -1;
}

static struct list_head send_buf_list = LIST_HEAD_INIT(send_buf_list);
static struct list_head send_unit_list = LIST_HEAD_INIT(send_unit_list);

static void add_send_buf_ref_count(struct send_buf *ptr)
{
	ptr->ref_count ++;
}

static void del_send_buf_ref_count(struct send_buf *ptr)
{
	ptr->ref_count --;
}

static struct send_buf *get_send_buf(char *buf, int buflen)
{
	struct send_buf *ptr = NULL;

	list_for_each_entry(ptr, &send_buf_list, entry) {
		if (ptr->buflen == buflen) {
			if (!memcmp(ptr->buf, buf, buflen)) {
				add_send_buf_ref_count(ptr);
				return ptr; 
			}
		}
	}

	ptr = malloc(sizeof(struct send_buf));
	if (ptr) {
		memset(ptr, 0 , sizeof(struct send_buf));
		ptr->buf = buf;
		ptr->buflen = buflen;
		add_send_buf_ref_count(ptr);
		list_add_tail(&ptr->entry, &send_buf_list);
	}
	return ptr;
}

static void del_send_buf(struct send_buf *ptr)
{
	if (ptr->ref_count == 0) {
		free(ptr->buf);
		list_del(&ptr->entry);
	}
	else {
		del_send_buf_ref_count(ptr);
	}
}

static void add_send_unit_ref(struct send_unit *ptr)
{
	ptr->ref_count ++;
}

static void del_send_unit_ref(struct send_unit *ptr)
{
	ptr->ref_count --;
}

static struct send_unit *get_send_unit(struct sockaddr_in *dest,
		struct sockaddr_in *source, char *buf, int buflen,
		callback cb, void *arg, int timeout, int retry)
{
	struct send_unit *ptr = malloc(sizeof(struct send_unit));

	if (ptr) {
		memset(ptr, 0, sizeof(struct send_unit));
		ptr->dest = *dest;
		ptr->source = *source;
		ptr->buf = get_send_buf(buf, buflen);
		ptr->cb = cb;
		ptr->arg = arg;
		ptr->timeout = timeout;
		ptr->retry = retry;
		add_send_unit_ref(ptr);
		list_add_tail(&ptr->entry, &send_unit_list);
	}
	return ptr;
}

static void free_send_unit(struct send_unit *ptr)
{
	if (ptr) {
		if (ptr->ref_count == 0) {
			del_send_buf(ptr->buf);
			list_del(&ptr->entry);
			free(ptr);
		}
		else {
			del_send_unit_ref(ptr);
		}
	}
}


#ifdef DEBUG_BT_SOCKET
struct event_base* base; 
struct timeval tv;
struct event *listen_ev;
void on_callback(int fd, short event, void *arg)
{
	if (event == EV_READ) {
		char buf[512] = {0};
		int len = recv(fd, buf, 512, 0);
		buf[len] = 0;

		printf("%s %s\n", __func__, buf);
	}
	else if (event == EV_TIMEOUT) {
		printf("%s why it has been called\n", __func__);
		event_add(listen_ev, &tv);
	}
}


int main(int argc, char **argv)
{
	struct sockaddr_in server_addr;
	int fd;

	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8888);

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bind(fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in));

	base = event_base_new();

	if (base) {
		struct event *tmp_ev = NULL;

		listen_ev = event_new(base, fd, EV_READ | EV_TIMEOUT | EV_PERSIST, on_callback, NULL);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		event_add(listen_ev, &tv); 

		tmp_ev = event_new(base, fd, EV_READ | EV_TIMEOUT, on_callback, NULL);
		event_add(tmp_ev, NULL); 

		event_base_dispatch(base); 
	}
	return 0; 
}

#endif /* DEBUG_BT_SOCKET */

