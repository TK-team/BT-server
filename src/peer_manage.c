#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>          /* See NOTES */
#include <netinet/in.h>
#include <unistd.h>

#include "peer_manage.h"
#include "peer_message.h"
//TODO: should pass the task struct pointer into peer_alloc()
struct peer_mgnt *peer_alloc(void)
{
	struct peer_mgnt *ptr = malloc(sizeof(struct peer_mgnt));

	memset(ptr, 0, sizeof(struct peer_mgnt));
	return ptr;
}

void peer_free(struct peer_mgnt *ptr)
{
	list_del(&ptr->head);
	b_string_free(ptr->peer_id);
	free(ptr);
}

struct peer_mgnt *peer_init(unsigned int ip, unsigned short port, struct b_string *peer_id, struct bt_task *task_ptr)
{
	struct peer_mgnt *ptr = peer_alloc();
	int sockfd = 0;
	struct sockaddr_in peer_addr;
	int ret = 0;

	ptr->ip = ip;
	ptr->port = port;
	ptr->peer_id = peer_id;
	ptr->task = task_ptr;

	/* Create socket for peer */
	ret = sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ret == -1) {
		TRACE(ERROR, "socket error, errno=(%d)\n", errno);
		goto ERR_DEAL;
	}
	peer_addr.sin_family = AF_INET;
	peer_addr.sin_port = htons(port);
	peer_addr.sin_addr.s_addr = htonl(ip);
	ret = connect(sockfd, (struct sockaddr *) (&peer_addr), sizeof(struct sockaddr_in));
	if (ret == -1) {
		TRACE(ERROR, "connect error, errno=(%d)\n", errno);
		close(sockfd);
		goto ERR_DEAL;
	}

	
	/* Create the timer */
	return ptr;
ERR_DEAL:
	peer_free(ptr);
	return NULL;

}

void peer_destory(struct peer_mgnt *ptr)
{

}

int main(int argc, char **argv)
{
	return 0;
}

