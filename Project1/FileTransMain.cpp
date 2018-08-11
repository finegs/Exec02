#ifndef __USE_GNU
#define __USE_GNU
#endif

#if 0

#include <signal.h>
#include <fcntl.h>

#if defined(_LINUX)
#include <sys/socket.h>
#else if defined(_WIN32) || defined(_WIN64)
#include <WinSock2.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#if defined(_LINUX)
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <iostream>

#include <map>
#include <vector>

#include <stdio.h>
#if defined(_LINUX)
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#define MAXDATALENTH	512


using namespace std;

int gpid;

// 클라이언트 데이터 정보 구조체
struct clidata
{
	int sockfd;			// 처리할 소켓 번호
	int signum;			// 처리할 RTS번호이자 처리할 쓰레드의 ID
	char cbuf[512];		// 처리해야할 데이터	

						// 아래로는 쓰이지 않는다.
						// 프로그램이 해야하는 일에 따라서 다양한 변수를 선언해서
						// 사용할 수 있을 것이다.
	int status;
	int uid;
	char regdate[36];
	char serviceinfo[256];
	char ip[36];
	char port[8];
};

map<int, struct clidata> ClientList;    // 처리할 클라이언트 데이타 정보 
map<int, struct clidata>::iterator CLi;

// 함수포인터 호출시 넘겨줄 데이터 클래스
// 역시 프로그램이 하는일에 따라서 다양하게 선언할 수 있을 것이다. 
class FuncArg
{
private:
	int status;
public:
};


typedef struct _fd_sig
{
	int signum;
	int pid;
	int uid;
} FdPerSignum;

pthread_mutex_t MutexLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  SyncCond = PTHREAD_COND_INITIALIZER;

// 쓰레드 풀관리를 위한 map 자료구조
multimap<int, FdPerSignum> PollMap;
multimap<int, FdPerSignum>::iterator PollMapi;

// 프로토콜에 대해서 호출될 함수포인터를 포함할 map 
typedef map<string, void *(*)(void *, class FuncArg *)> __ProcessingMap;
__ProcessingMap ProcessingMap;
__ProcessingMap::iterator ProcessingFunci;

// login 문자열을 만났을 때 호출할 함수
void *ProcessLogin(void *data, class FuncArg *aFuncArg)
{
	struct clidata lclidata;
	lclidata = *(struct clidata *)data;
	printf("Read socket(%d) : LoginProcess : %s\n", lclidata.sockfd,
		lclidata.cbuf);
}

// list 문자열을 만났을 때 호출할 함수
void *ProcessList(void *data, class FuncArg *aFuncArg)
{
	struct clidata lclidata;
	lclidata = *(struct clidata *)data;
	printf("Read socket(%d) : ListProcess : %s\n", lclidata.sockfd,
		lclidata.cbuf);
}

// down 문자열을 만났을 때 호출할 함수
void *ProcessDown(void *data, class FuncArg *aFuncArg)
{
	struct clidata lclidata;
	lclidata = *(struct clidata *)data;
	printf("Read socket(%d) : DownProcess : %s\n", lclidata.sockfd,
		lclidata.cbuf);
}

// quit 문자열을 만났을 때 호출할 함수
// 소켓을 종료한다. 
void *ProcessQuit(void *data, class FuncArg *aFuncArg)
{
	struct clidata lclidata;
	lclidata = *(struct clidata *)data;
	printf("Read socket(%d) : QuitProcess : %s\n", lclidata.sockfd,
		lclidata.cbuf);
	close(lclidata.sockfd);
}

// 파일지정자가 리얼타임 시그널에 대응하도록 설정한다.
int setup_sigio(int fd, int sig_num, int pid)
{
	if (fcntl(fd, F_SETFL, O_RDWR | O_NONBLOCK | O_ASYNC) < 0)
	{
		printf("setup nonblocking error:%d\n", fd);
	}

	if (fcntl(fd, F_SETSIG, SIGRTMIN + sig_num) < 0)
	{
		printf("Couldn't set signal %d on %d\n", SIGRTMIN, fd);
		return -1;
	}

	if (fcntl(fd, F_SETOWN, pid) < 0)
	{
		printf("Couldn't set owner %d on %d\n", pid, fd);
		return -1;
	}
	return 0;
}

// 클라이언트로 부터 읽어들인 데이터를 처리할 쓰레드 함수
void * CliDataProcessing(void *rts_num)
{
	int signum = *((int *)rts_num);
	socklen_t socklen;
	char buf[MAXDATALENTH];
	class FuncArg FArg;

	struct sockaddr_in sockaddr;
	struct siginfo sinfo;

	FdPerSignum fdpersignum;

	int clientnum;
	int ret;
	int readlen;
	int datalen;

	sigset_t sigset;

	gpid = getpid();
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGRTMIN + signum);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	pthread_mutex_lock(&MutexLock);
	usleep(500);
	pthread_cond_signal(&SyncCond);
	pthread_mutex_unlock(&MutexLock);

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGRTMIN + signum);
	sigprocmask(SIG_BLOCK, &sigset, NULL);
	while (1)
	{
		struct clidata lclidata;
		socklen = sizeof(sockaddr);

		// 쓰레드가 처리하기로 지정된 소켓에서 리얼타임 시그널이 발생하는걸 기다린다.
		ret = sigwaitinfo(&sigset, &sinfo);
		if (ret == (SIGRTMIN + signum))
		{
			// 받을 데이터 크기를 읽어온다
			if ((readlen = read(sinfo.si_fd, (void *)&datalen, sizeof(int))) <= 0)
			{
				// 만약 데이터를 읽는 도중 에러가 발생했다면, 
				// 소켓을 닫고
				// 쓰레드풀의 값을 재조정한다.
				PollMapi = PollMap.begin();
				while (PollMapi != PollMap.end())
				{
					if (PollMapi->second.signum == signum)
					{
						fdpersignum = PollMapi->second;
						if (PollMapi->first > 0)
							clientnum = PollMapi->first - 1;
						PollMap.erase(PollMapi);
						PollMap.insert(pair<int, FdPerSignum>(clientnum, fdpersignum));
					}
					PollMapi++;
				}
				close(sinfo.si_fd);
			}
			else
			{
				// 클라이언트로 부터 데이터를 읽어온다.
				void *(*Func)(void *, class FuncArg *);
				int readn;
				memset(buf, 0x00, MAXDATALENTH);
				readn = read(sinfo.si_fd, buf, datalen + 4);
				memset(lclidata.cbuf, 0x00, sizeof(lclidata.cbuf));
				memcpy(lclidata.cbuf, buf, datalen);

				if ((CLi = ClientList.find(sinfo.si_fd)) == ClientList.end())
				{
					lclidata.sockfd = sinfo.si_fd;
					lclidata.signum = signum;
					lclidata.status = 0;
					ClientList[sinfo.si_fd] = lclidata;
				}

				// 처리해야할 메시지에 대한 함수가 존재할 때
				// 해당 함수를 호출한다.
				ProcessingFunci = ProcessingMap.find(buf);
				if (ProcessingFunci != ProcessingMap.end())
				{
					cout << "Search Function data " << endl;
					Func = ProcessingFunci->second;
					Func((void *)&lclidata, &FArg);
				}
				else
				{
					fprintf(stderr, "Unknown msg protocol : %s\n", buf);
				}
			}
		}
	}
}

void * socket_acceptor(void *argdata)
{
	int server_sockfd, cli_sockfd;
	int clientnum;
	FdPerSignum fdpersignum;
	socklen_t socklen;

	sigset_t sigset;
	struct sockaddr_in serveraddr, clientaddr;
	struct siginfo sinfo;

	int signum = *((int *)argdata);

	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error ");
	}
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(5555);

	if (bind(server_sockfd, (struct sockaddr *)&serveraddr,
		sizeof(serveraddr)) == -1)
	{
		perror("bind error");
	}

	if (listen(server_sockfd, 5) == -1)
	{
		perror("listen error");
	}

	if (setup_sigio(server_sockfd, signum, getpid()) == -1)
	{
		fprintf(stderr, "sigio setting error\n");
	}

	gpid = getpid();
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGRTMIN + signum);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	pthread_mutex_lock(&MutexLock);
	usleep(500);
	pthread_cond_signal(&SyncCond);
	pthread_mutex_unlock(&MutexLock);

	while (1)
	{
		int ret;
		printf("Wait I/O Event (%d)\n", signum);
		ret = sigwaitinfo(&sigset, &sinfo);
		printf("IO Event OK\n");
		if (ret == SIGRTMIN + signum)
		{
			printf("Accept \n");
			socklen = sizeof(clientaddr);
			cli_sockfd = accept(server_sockfd,
				(struct sockaddr *)&clientaddr,
				&socklen);

			if (cli_sockfd < 0)
			{
				fprintf(stderr, "Accept error\n");
				continue;
			}
			PollMapi = PollMap.begin();
			fdpersignum = PollMapi->second;
			clientnum = PollMapi->first + 1;
			setup_sigio(cli_sockfd, PollMapi->second.signum, PollMapi->second.pid);
			cout << "할당 쓰레드 번호 " << PollMapi->second.signum << endl;
			PollMap.erase(PollMapi);

			PollMap.insert(pair<int, FdPerSignum>(clientnum, fdpersignum));
		}
		else
		{
			// 에러처리
			cout << "Unknown Error : " << ret << endl;
		}
	}
}

// 각 프로토콜을 처리할 함수를 등록한다.
int ProtocolFuncRegist()
{
	ProcessingMap["login"] = ProcessLogin;
	ProcessingMap["list"] = ProcessList;
	ProcessingMap["down"] = ProcessDown;
	ProcessingMap["quit"] = ProcessQuit;
}

int main(int argc, char **argv)
{

	FdPerSignum fdpersignum;
	int status;
	int loopi, loopj;
	sigset_t sigset;
	sigaddset(&sigset, SIGRTMIN);

	int thread_num = 2;              // 쓰레드 풀의 크기
	vector<void *(*)(void *)> thread_list;    // 쓰레드 리스트
	vector<pthread_t> threadident(thread_num);  // 쓰레드 ID 저장

												// 생성될 쓰레드의 갯수만큼 리얼타임 시그널을 등록한다. 
	for (loopi = 0; loopi < thread_num + 1; loopi++)
	{
		sigaddset(&sigset, SIGRTMIN + (loopi + 1));
	}

	// 등록된 리얼타임 시그널이 발생할경우 블럭되도록 설정한다.
	sigprocmask(SIG_BLOCK, &sigset, NULL);
	// 각 프로토콜을 처리할 함수를 등록한다. 
	ProtocolFuncRegist();

	// 클라이언트의 연결을 기다리는 쓰레드를 등록한다. 
	thread_list.push_back(socket_acceptor);

	// 클라이언트의 데이터를 처리하는 쓰레드를 등록한다.
	for (loopi = 0; loopi < thread_num; loopi++)
	{
		thread_list.push_back(CliDataProcessing);
	}

	for (loopi = 0, loopj = 1; loopi < thread_list.size(); loopi++, loopj++)
	{
		pthread_mutex_lock(&MutexLock);
		pthread_create(&threadident[loopi], NULL,
			thread_list[loopi],
			(void *)&loopj);

		pthread_cond_wait(&SyncCond, &MutexLock);

		fdpersignum.signum = loopj;
		fdpersignum.pid = gpid;
		cout << "PID is " << gpid << endl;
		if (loopi != 0)
			PollMap.insert(pair<int, FdPerSignum>(0, fdpersignum));

		pthread_mutex_unlock(&MutexLock);
	}

	for (loopi = 0; loopi < thread_list.size(); loopi++)
	{
		pthread_join(threadident[loopi], (void **)&status);
	}

	return 1;
}


#endif