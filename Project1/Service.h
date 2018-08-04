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

	typedef struct UDPService_ UDPService;
	typedef struct UDPService_ {
		int mode; // 1 : Passive, 2 : Acivive
		int run;  // 1 : Run, 0 : Stop, 2 : Pause
		char* ip;
		int port;

		SOCKET* sockfd;

		int (*recv)(UDPService* udpSvc, byte* recvBuff);
		int (*send)(UDPService* udpSvc, byte* recvBuff);
	} UDPService;

	int addUDPServer(UDPService* server);
	int removeUDPServer(UDPService* server);

	int startUDPService(UDPService* svc);
	int stopUDPService(UDPService* svc);

	int connectUDPServer(const char *ip, const int port);
	int disconnectUDPServer(const char *ip, const int port);

#if __CPLUSCPLUS
}
#endif

#endif