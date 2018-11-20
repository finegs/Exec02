#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64) 
#include <winsock.h>
#include <windows.h>
#else 
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
#define PORT 20009
#define BUF_LEN 100
/* simple upd server
this program receives short messages (<99 characters) from any IP address
and writes them to the display
be sure to use the linker line option "-l wsock32"
*/
int main(int argc, char* argv[])
{
	/* first define a socket
	a socket is an I/O port like a file descriptor
	*/
#if defined(_WIN32) || defined(_WIN64) 
	SOCKET sock;   /* SOCKET is a typedef for a structure */
#else
	int sock;
#endif
	int size;
	int nbytes, flags;
#if defined(_WIN32) || defined(_WIN64) 
	int addrlen;
#else
	socklen_t addrlen;
#endif
	int i;
	/* char loopback[20]="127.0.0.1"; */
#if defined(_WIN32) || defined(_WIN64) 
	WSADATA wsaData;              /* This is struct holds Windows required data */
	int nCode;
#endif
    

	char buffer[BUF_LEN];
	struct sockaddr_in server;    /* this holds my IP address and port info */
	struct sockaddr_in from;      /* this holds the same info for the sender of the packet
								  I received */
								  /* the call to WSAStartup is Windows magic */
#if defined(_WIN32) || defined(_WIN64) 
	if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
		printf("Opps! WSA error %u\n", nCode);
		return -1;
	}
#endif
	/* create a socket called sock. It is a datagram socket */
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		printf("socket error = %llu\n", sock);
		return -1;
	}
	server.sin_family = AF_INET;   /* initialize the server address family */
	server.sin_addr.s_addr = htonl(INADDR_ANY); /* notice this struct within a struct */
												/* printf("%x\n",server.sin_addr.s_addr); */
	server.sin_port = htons(PORT);
	/* associate the socket with the address structure - this is called binding */
	i = bind(sock, (struct sockaddr *) &server, sizeof(server));
	if (i < 0) {
		printf("bind result: %d\n", i);
		return -1;
	}
	else
		printf("Simple UDP server is ready!\n\n");
	nbytes = 99; /* receive packets up to 99 bytes long */
	flags = 0;  /* must be zero or this will not work! */
	while (1) {
		/* the recvfrom function is a read and the arguments are:
		sock - the socket we are reading
		buffer - array into which to read the data
		nbytes - read up to this many bytes
		flags - used for special purposes - not needed here
		from - sockaddr struct to hold the IP address and port of the sender of the packet
		addrlen - the size of the sockaddr struct written by this function
		*/
		addrlen = sizeof(from);
		size = recvfrom(sock, buffer, nbytes, flags, (struct sockaddr *)&from, &addrlen);
		if ((size > 0) && (size < BUF_LEN)) {
			buffer[size] = '\0';      /* add the null byte so buffer now holds a string */
			i = puts((char *)buffer);    /* write this string to the display */
            //fflush(stdout);
		}

		//echo message back to client

		if (sock < 0) {//
			printf("socket error = %llu\n", sock);//
			return -1;//
		}//

		sendto(sock, buffer, nbytes, flags, (struct sockaddr *)&from, addrlen); //
	}

#if defined(_WIN32) || defined(_WIN64) 
	system("PAUSE");
#endif
	return 0;
}