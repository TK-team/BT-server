/* A requirement for listening UDP port 30000. */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifdef _UNIT_TEST
#include "unittest/cmockery.h"
#endif

#define BUF_LEN 8192
#define DEFAULT_SERVER_IP 0xc0a80001
#define DEFAULT_SERVER_PORT 30000

int init_udp_server(unsigned int ip, unsigned short port)
{
	struct sockaddr_in server_ip;
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	char buf[BUF_LEN];
	unsigned int len = 0;
	unsigned int n = 0;

	if (fd < 0) {
		goto err_deal;
	}

	server_ip.sin_family = AF_INET;
	server_ip.sin_addr.s_addr = htonl(INADDR_ANY);
	server_ip.sin_port = htons(port);
	bind(fd, (struct sockaddr*)&server_ip, sizeof(server_ip));

	do {
		len = BUF_LEN;
		n = 0;
		memset(buf, 0, BUF_LEN);
		n = recvfrom(fd, buf, BUF_LEN, 0, (struct sockaddr*)&server_ip, &len);
		puts(buf);
	} while (1);

	return 0;
err_deal:
	return -1;
}

void usage(void)
{
	char help[] = "udp_server [ip] [port]\n"
		"Example: udp_server 192.168.0.1 30000\n";
	printf("%s\n", help);
}

// A test case that does nothing and succeeds.
void null_test_success(void **state) {
}

int main(int argc, char **argv)
{
	unsigned short port = DEFAULT_SERVER_PORT;
	struct in_addr ip;

#ifdef _UNIT_TEST
	const UnitTest tests[] = {
	    unit_test(null_test_success),
	};
	run_tests(tests);
#endif
	ip.s_addr = DEFAULT_SERVER_IP;
	if (argc == 1) {
		printf("udp_server use default ip and port: 192.168.0.1 30000\n");
		init_udp_server((unsigned int)(ip.s_addr), port);
	} else if (argc == 2) {
		if (inet_aton(argv[1], &ip) == 0) {
			perror("inet_aton error\n");
			usage();
			exit(EXIT_FAILURE);
		}
		init_udp_server(ntohl((unsigned int)(ip.s_addr)), port);

	} else if (argc == 3) {
		if (inet_aton(argv[1], &ip) == 0) {
			perror("inet_aton error\n");
			usage();
			exit(EXIT_FAILURE);
		}
		port = atoi(argv[2]);
		init_udp_server(ntohl((unsigned int)(ip.s_addr)), port);
	} else {
		usage();
	}
	exit(EXIT_SUCCESS);
}

