/*
 * =====================================================================================
 *
 *	Filename:  udp_client.c
 *
 *	Description:  
 *
 *	Version:  1.0
 *	Created:  2015年07月25日 20时51分55秒
 *	Revision:  none
 *	Compiler:  gcc
 *
 *	Author:  kuangguiming, lawrence_kuang@gmail.com
 *	Company:  
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define SERVER_IP	"127.0.0.1"
#define SERVER_PORT	6547
#define RECV_BUF_SIZE	4096

#define DEBUG 1
#ifdef DEBUG
#define DEBUG_PRINT	printf
#else
#define DEBUG_PRINT
#endif

extern int errno;
extern char *optarg;
extern int optind, opterr, optopt;

static void usage(FILE *fop, int exitval)
{
	fprintf(fop, "\
cli_udp: [-sp message] [-h]\n\
		    -s server ip\n\
		    -p server port\n\
		    -h -- print help info\n\
"
		   );

	exit(exitval);
}

int main(int argc, char *argv[])
{
	int cli_sock;
	struct sockaddr_in srv_addr;
	char *srv_ip;
	int srv_port;
	char recvbuf[RECV_BUF_SIZE];
	char c;

	if (argc < 2)
		usage(stderr, 1);


	while ((c = getopt(argc, argv, "sph")) != EOF)
	{
		switch (c) {
		case 's':
			srv_ip = argv[optind++];
			DEBUG_PRINT("server ip:%s\n", srv_ip);
			break;

		case 'p':
			srv_port = atoi(argv[optind++]);
			DEBUG_PRINT("server port:%d\n", srv_port);
			break;

		case 'h':
			usage(stderr, 0);
			break;

		default:
			usage(stderr, 1);
			break;
		}
	}

	DEBUG_PRINT("message: %s\n", argv[optind]);

	cli_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (cli_sock < 0)
	{
		printf("socket create failed: %s\n", strerror(errno)); 
		return errno;
	}

	memset(&srv_addr, 0, sizeof(struct sockaddr_in));

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr(srv_ip);
	srv_addr.sin_port = htons(srv_port);

	if (connect(cli_sock, (struct sockaddr*)&srv_addr, (socklen_t)(sizeof(struct sockaddr))))
	{
		printf("connect failed: %s\n", strerror(errno)); 
		goto out;
	}

	if (send(cli_sock, argv[optind], (size_t)(strlen(argv[optind])), 0) != (size_t)(strlen(argv[1])))
	{
		printf("send failed: %s\n", strerror(errno)); 
		goto out;
	}

	if (recv(cli_sock, recvbuf, RECV_BUF_SIZE, 0) < 0)
	{
		printf("recv failed: %s\n", strerror(errno)); 
		goto out;
	}

out:
	if (cli_sock)
		close(cli_sock);

	return 0;
}


