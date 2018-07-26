#ifndef _SERVICE_H
#define _SERVICE_H

#if __CPLUSPLUS
extern "C" {
#endif

	typedef struct UDPServer_ {
		const char* ip;
		const int port;
	} UDPServer;

	int addUDPServer(UDPServer* server);
	int removeUDPServer(UDPServer* server);

	int startUDPServer(const char *ip, const int port);
	int stopUDPServer(const char *ip, const int port);

	int connectUDPServer(const char *ip, const int port);
	int disconnectUDPServer(const char *ip, const int port);


#if __CPLUSCPLUS
}
#endif

#endif