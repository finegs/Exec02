#include <stdio.h>
#include <WinSock2.h>

#include "service.h"

#define MAXLINE 1024

int addUDPServer(UDPService* server) {
	return 0;
}

int removeUDPServer(UDPService* server) {
	return 0;
}

int startUDPServer(SOCKET *sockfd, const char *ip, const int port, int(*handleRecvBuffer)(SOCKET *sfd, const char* recvBuf)) {
	//int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from server";
	struct sockaddr_in servaddr, cliaddr;

	// Creating socket file descriptor
	if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		//exit(EXIT_FAILURE);
		return EXIT_FAILURE;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	// Bind the socket with the server address
	if (bind(*sockfd, (const struct sockaddr *)&servaddr,
		sizeof(servaddr)) < 0)
	{
		perror("bind failed");
		//exit(EXIT_FAILURE);
		return EXIT_FAILURE;
	}

	int len, n;

	n = recvfrom(*sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);

	buffer[n] = '\0';
	
	handleRecvBuffer(sockfd, buffer);

	return 0;
}

int stopUDPServer(SOCKET *sockfd, const char *ip, const int port, int(*handleRecvBuffer)(SOCKET *sfd, const char* recvBuf)) {
	return 0;
}

int startUDPService(UDPService* svc) {
	startUDPServer(svc->sockfd, svc->ip, svc->port, svc->recv);
}
int destroyUDPService(UDPService* svc) {
	stopUDPServer(svc->sockfd, svc->ip, svc->port, NULL);
}
 
int connectUDPServer(const char *ip, const int port) {
	return 0;
}

int disconnectUDPServer(const char *ip, const int port) {
	return 0;
}
