/* Author:	Tristan Xiao
 * Email:	tristan_xiao@qq.com
 * Date:	2015.12.2
 * Description:	The basic send receive service for upper layer,
 * 		it should sheild the native socket API in the glibc.
 */
#include <netinet/in.h>

/*	Structure of the modules:
 *
 *	Upper layer
 *	--------------------------------------
 *	bt_socket
 *	--------------------------------------
 *	libevent/libev/libuv/native socket API
 *
 */

enum {
	BT_IPPROTO_TCP = 100,
	BT_IPPROTO_UDP,
};

typedef void (*callback)(int fd, short event, void *arg);

 /*	Description:	send buf in the synchronous way.
  *	params:		dest - the IP and port of destination;
  *	params:		buf - buffer to be sent;
  *	params:		buflen - lenght of sent buffer;
  *	params:		timeout - timeout of waiting receiving data after send buffer;
  *	params:		flag - TCP or UDP;
  *	params:		recv - point to the receive buffer;
  *	return:		0 - send successful but no recv data;
  			-1 - send failed;
			> 0 - recv buffer length;
  */


extern int bt_sync_send(struct sockaddr_in *dest, struct sockaddr_in *source, char *buf,
		int buflen, int timeout, int flag, char **recv);
 /*	Description:	send buf in the asynchronous way.
  *	params:		dest - the IP and port of destination;
  *	params:		buf - buffer to be sent;
  *	params:		buflen - lenght of sent buffer;
  *	params:		timeout - timeout of waiting receiving data after send buffer;
  *	params:		flag - TCP or UDP;
  *	params:		callback - point to the receive buffer;
  *	return:		0 - send successful but no recv data;
  			-1 - send failed;
  */
extern int bt_async_send(struct sockaddr_in *dest, char *buf, int buflen, int timeout, int flag,
		callback);


