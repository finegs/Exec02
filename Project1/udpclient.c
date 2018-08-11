#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) || defined(_WIN64) 
//#include <WinSock2.h>
#include <windows.h>
//#include <winsock.h>
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
#define BUF_LEN 100
#define ADDR_LEN 20

int hostnameToIp(char* hostname, char* ip) {
    struct hostent *he;
    struct in_addr **addr_list;

    int i;

    if ((he = gethostbyname(hostname)) == NULL) {
        fprintf(stderr, "gethostbyname : failed, hostname=%s\n", hostname);
        return 1;
    }

    addr_list = (struct in_addr**)he->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++) {
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return 0;
    }
    return 1;
}

int printSocketErrorDetail() {
    int errCode = WSAGetLastError();

    // ..and the human readable error string!!
    // Interesting:  Also retrievable by net helpmsg 10060
    LPSTR errString = NULL;  // will be allocated and filled by FormatMessage

    int size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM, // use windows internal message table
        0,       // 0 since source is internal message table
        errCode, // this is the error code returned by WSAGetLastError()
                 // Could just as well have been an error code from generic
                 // Windows errors from GetLastError()
        0,       // auto-determine language to use
        (LPSTR)&errString, // this is WHERE we want FormatMessage
                           // to plunk the error string.  Note the
                           // peculiar pass format:  Even though
                           // errString is already a pointer, we
                           // pass &errString (which is really type LPSTR* now)
                           // and then CAST IT to (LPSTR).  This is a really weird
                           // trip up.. but its how they do it on msdn:
                           // http://msdn.microsoft.com/en-us/library/ms679351(VS.85).aspx
        0,                 // min size for buffer
        0);               // 0, since getting message from system tables
    printf("Error code %d:  %s\n\nMessage was %d bytes, in case you cared to know this.\n\n", errCode, errString, size);

    LocalFree(errString); // if you don't do this, you will get an
                          // ever so slight memory leak, since we asked
                          // FormatMessage to FORMAT_MESSAGE_ALLOCATE_BUFFER,
                          // and it does so using LocalAlloc
                          // Gotcha!  I guess.

}

/* simple upd client */
int main()
{
#if defined(_WIN32) || defined(_WIN64) 
	SOCKET sock;
#else
	int sock;
#endif
	size_t size;
    size_t nbytes;
    int flags;
	int i;
	char * cp;
#ifdef WIN
	WSADATA wsaData;
	int nCode;
#endif
	char buffer[BUF_LEN];
	char str_target_addr[ADDR_LEN], str_addr_temp[ADDR_LEN];        /* holds the chars of an IP address */
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
	sock = socket(PF_INET, SOCK_DGRAM , IPPROTO_UDP);
	if (sock < 0) {
		printf("socket error = %llu\n", sock);
		return -1;
	}
	/* we fill in the address family and port, but we do not know the destination IP address yet */
    memset((char*)&target_pc, 0, sizeof(target_pc));   
	target_pc.sin_family = PF_INET;
	target_pc.sin_port = htons(PORT);

	/* fill in my address and port */
	me.sin_family = PF_INET;
	me.sin_port = htons(0);
	me.sin_addr.s_addr = htonl(INADDR_ANY);

	//i = connect(sock, (struct sockaddr *) &me, sizeof(me));
	//if (i < 0) {
	//	printf("bind result: %d\n", i);
	//	return -1;
	//}

	nbytes = BUF_LEN-1;

    memset(str_target_addr, '\0', ADDR_LEN);

	while (1) {
        memset(buffer, '\0', BUF_LEN);

		printf("Enter the target IP address: ");
        strcpy(str_addr_temp, str_target_addr);
		cp = fgets(str_target_addr, ADDR_LEN-1, stdin);

        if (NULL != cp && strlen(cp) > 0 && cp[0] != '\n') { // Not NULL && cp[0] != '\n'

            /* remove the \n */

            if (str_target_addr[strlen(str_target_addr) - 1] == '\n')
                str_target_addr[strlen(str_target_addr) - 1] = '\0';

            if (strlen(str_target_addr) > 0 
                && (str_target_addr[0] < '0' || str_target_addr[0] > '9')) {
                memset(str_addr_temp, '\0', ADDR_LEN);
                if (!hostnameToIp(str_target_addr, str_addr_temp)) {
                    memcpy(str_target_addr, str_addr_temp, strlen(str_addr_temp));
                }
                else {
                    fprintf(stderr, "hostnameToIp : Unknown hostname=%s\n", str_target_addr);
                    continue;
                }

            }
            /* the inet_addr function converts a string form of IP address to a 32 binary integer */
            target_pc.sin_addr.S_un.S_addr = inet_addr(&str_target_addr[0]);

        }
        else {
            memset(str_target_addr, '\0', ADDR_LEN);
            strcpy(str_target_addr, str_addr_temp);
            target_pc.sin_addr.S_un.S_addr = inet_addr(&str_addr_temp[0]);
        }
		//target_pc.sin_addr.s_addr = inet_pton(&str_target_addr[0]);

		printf("Enter your message: ");
		cp = fgets(buffer, BUF_LEN-1, stdin);
		/* get the string length so we send exactly this many characters */
		//nbytes = strlen(buffer);
		flags = 0;

        if (buffer[nbytes - 1] == '\n') {
            buffer[nbytes - 1] = '\0';
            nbytes--;
        }

		size = sendto(sock, (char *)buffer, (int)nbytes, flags, (struct sockaddr *)&target_pc, sizeof(target_pc));
        
		printf("Sendto : Msg = %s, Len = %d, Size = %d\n", buffer, (int)nbytes, (int)size);
        if (size <= 0) {
            printSocketErrorDetail();
            continue;
        }

		//added
		int addrlen = sizeof(target_pc);
        memset(buffer, '\0', BUF_LEN);
		size = recvfrom(sock, buffer, (int)nbytes, flags, (struct sockaddr *)&target_pc, &addrlen);
		if ((size > 0) && (size < BUF_LEN-1)) {
			buffer[size] = '\0';      //add the null byte so buffer now holds a string
            printf("Recvfrom : Msg = %s, Len = %llu, Size = %llu\n", buffer, strlen(buffer), size);
			//i = puts((char *)buffer);    // write this string to the display 
		}
	}

    WSACleanup();

#if defined(_WIN32) || defined(_WIN64) 
	system("PAUSE");
#endif

    printf("Finish UDPClient : close UDP Socket, Address=%s, PortNo = %d\n", inet_ntoa(me.sin_addr), me.sin_port);

	return 0;
}