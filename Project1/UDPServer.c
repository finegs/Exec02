#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define MAXBUF 1024*1024

void echo(int sd, int (*handle)(char* rcvbuf, int rcvBufLen)) {
	int len, n;
	char bufin[MAXBUF];
	struct sockaddr_in remote;

	len = sizeof(remote);

	while (1) {
		memset(bufin, 0, MAXBUF);
		memset(&remote, 0, len);
		n = recvfrom(sd, bufin, MAXBUF, 0, (struct sockaddr *)&remote, &len);

		if (n < 0) {
			perror("error : receiveing data\n");
		}
		else {
			handle(bufin, n);
			printf("recv (%s:%d) : %s\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port), bufin);
		}
	}
}

int start(const int port, int* ld) {
	struct sockaddr_in sockaddr;
	int length;

	if ((*ld = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		pritf("error : creating socket");
		return -1;
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(port);

	if (bind(*ld, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		printf("error : binding, port=%d\n", port);
		return -2;
	}

	length = sizeof(sockaddr);
	if (getsockname(*ld, (struct sockaddr*)&sockaddr, &length) < 0) {
		printf("error : getsockname\n");
		return -3;
	}

	printf("info : server UDP port=%d\n", ntohs(sockaddr.sin_port));
	return 0;
}

#endif