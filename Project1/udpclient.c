#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) || defined(_WIN64) 
#include <winsock.h>
#include <windows.h>
#else 
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
/* Here are some details of the sockaddr_in structure and the sockaddr structure
These declarations are copied from winsock.h

struct in_addr {       this struct holds a 32 bit IP address
union {
struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
struct { u_short s_w1,s_w2; } S_un_w;
u_long S_addr;
} S_un;
#define s_addr  S_un.S_addr

struct sockaddr_in {   notice this structure is 16 bytes long
short       sin_family;
u_short     sin_port;
struct      in_addr sin_addr;
char        sin_zero[8];
};
struct sockaddr {       this generic address structure is 16 bytes long, too!
u_short sa_family;
char        sa_data[14];
};

*/
/* we have to send on the same port the server is listening on */
#define PORT 20009
/* simple upd client */
int main()
{
#if defined(_WIN32) || defined(_WIN64) 
	SOCKET sock;
#else
	int sock;
#endif
	int size;
	int nbytes, flags;
	int i;
	char * cp;
#ifdef WIN
	WSADATA wsaData;
	int nCode;
#endif
	char buffer[100];
	char str_addr[20];        /* holds the chars of an IP address */
	struct sockaddr_in target_pc, me;

	/* magic call to initialize the network I/O code - only Microsoft requires this */
#if defined(_WIN32) || defined(_WIN64) 
	int nCode;
	LPWSADATA wsaData = (LPWSADATA)malloc(sizeof(LPWSADATA));
	if ((nCode = WSAStartup(MAKEWORD(2, 2), wsaData)) != 0) {
		printf("Opps! WSA error %d\n", nCode);
		return -1;
	}
#endif
	/* create a socket to send on */
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("socket error = %ld\n", sock);
		return -1;
	}
	/* we fill in the address family and port, but we do not know the destination IP address yet */
	target_pc.sin_family = PF_INET;
	target_pc.sin_port = htons(PORT);
	/* fill in my address and port */
	me.sin_family = PF_INET;
	me.sin_port = htons(0);
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	i = bind(sock, (struct sockaddr *) &me, sizeof(me));
	if (i < 0) {
		printf("bind result: %d\n", i);
		return -1;
	}


	nbytes = 99;

	while (1) {
		printf("Enter the target IP address: ");
		cp = fgets(str_addr, 19, stdin);
		/* remove the \n */
		str_addr[strlen(str_addr) - 1] = '\0';
		/* the inet_addr function converts a string form of IP address to a 32 binary integer */
		target_pc.sin_addr.s_addr = inet_addr(&str_addr[0]);
		printf("Enter your message: ");
		cp = fgets(buffer, 99, stdin);
		/* get the string length so we send exactly this many characters */
		nbytes = strlen(buffer);
		flags = 0;
		size = sendto(sock, (char *)buffer, nbytes, flags, (struct sockaddr *)&target_pc, sizeof(target_pc));
		printf("msg size = %d size = %d\n", nbytes, size);

		//added

		int addrlen = sizeof(target_pc);
		size = recvfrom(sock, buffer, nbytes, flags, (struct sockaddr *)&target_pc, &addrlen);
		if ((size > 0) && (size < 99)) {
			buffer[size] = '\0';      //add the null byte so buffer now holds a string 
			i = puts((char *)buffer);    // write this string to the display 
		}

	}
#if defined(_WIN32) || defined(_WIN64) 
	system("PAUSE");
#endif
	return 0;
}