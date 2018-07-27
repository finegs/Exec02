#ifndef _SERVICE_H
#define _SERVICE_H

#if __CPLUSPLUS
extern "C" {
#endif

	typedef struct UDPService_ {
		const char* ip;
		const int port;

		int (*recv)(int* sockfd, byte recvBuff[]);
		int (*send)(int* sockfd, byte sendBuff[]);
	} UDPService;

	int addUDPServer(UDPService* server);
	int removeUDPServer(UDPService* server);

	int startUDPServer(SOCKET *sockfd, const char *ip, const int port, int(*handleRecvBuffer)(SOCKET *sfd, const char* recvBuf));
	int stopUDPServer(SOCKET **sockfd, const char *ip, const int port);

	int connectUDPServer(const char *ip, const int port);
	int disconnectUDPServer(const char *ip, const int port);


#if __CPLUSCPLUS
}
#endif

#endif