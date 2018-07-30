#ifndef _SERVICE_H
#define _SERVICE_H

#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64) 
//#include <winsock.h>
#include <WinSock2.h>
#include <windows.h>
#else 
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#if __CPLUSPLUS
extern "C" {
#endif

	typedef struct UDPService_ {
		int mode; // 1 : Passive, 2 : Acivive
		char* ip;
		int port;

		SOCKET* sockfd;

		int (*recv)(int* sockfd, byte* recvBuff, const char* ip, const char* port);
		int (*send)(int* sockfd, byte* sendBuff, const char* ip, const char* port);
	} UDPService;

	int addUDPServer(UDPService* server);
	int removeUDPServer(UDPService* server);

	int startUDPServer(SOCKET *sockfd, const char *ip, const int port, int(*handleRecvBuffer)(SOCKET *sfd, const char* recvBuf));
	int stopUDPServer(SOCKET *sockfd, const char *ip, const int port, int(*handleRecvBuffer)(SOCKET *sfd, const char* recvBuf));

	int startUDPService(UDPService* svc);
	int destroyUDPService(UDPService* svc);

	int connectUDPServer(const char *ip, const int port);
	int disconnectUDPServer(const char *ip, const int port);


#if __CPLUSCPLUS
}
#endif

#endif