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

// Ŭ���̾�Ʈ ������ ���� ����ü
struct clidata
{
	int sockfd;			// ó���� ���� ��ȣ
	int signum;			// ó���� RTS��ȣ���� ó���� �������� ID
	char cbuf[512];		// ó���ؾ��� ������	

						// �Ʒ��δ� ������ �ʴ´�.
						// ���α׷��� �ؾ��ϴ� �Ͽ� ���� �پ��� ������ �����ؼ�
						// ����� �� ���� ���̴�.
	int status;
	int uid;
	char regdate[36];
	char serviceinfo[256];
	char ip[36];
	char port[8];
};

map<int, struct clidata> ClientList;    // ó���� Ŭ���̾�Ʈ ����Ÿ ���� 
map<int, struct clidata>::iterator CLi;

// �Լ������� ȣ��� �Ѱ��� ������ Ŭ����
// ���� ���α׷��� �ϴ��Ͽ� ���� �پ��ϰ� ������ �� ���� ���̴�. 
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

// ������ Ǯ������ ���� map �ڷᱸ��
multimap<int, FdPerSignum> PollMap;
multimap<int, FdPerSignum>::iterator PollMapi;

// �������ݿ� ���ؼ� ȣ��� �Լ������͸� ������ map 
typedef map<string, void *(*)(void *, class FuncArg *)> __ProcessingMap;
__ProcessingMap ProcessingMap;
__ProcessingMap::iterator ProcessingFunci;

// login ���ڿ��� ������ �� ȣ���� �Լ�
void *ProcessLogin(void *data, class FuncArg *aFuncArg)
{
	struct clidata lclidata;
	lclidata = *(struct clidata *)data;
	printf("Read socket(%d) : LoginProcess : %s\n", lclidata.sockfd,
		lclidata.cbuf);
}

// list ���ڿ��� ������ �� ȣ���� �Լ�
void *ProcessList(void *data, class FuncArg *aFuncArg)
{
	struct clidata lclidata;
	lclidata = *(struct clidata *)data;
	printf("Read socket(%d) : ListProcess : %s\n", lclidata.sockfd,
		lclidata.cbuf);
}

// down ���ڿ��� ������ �� ȣ���� �Լ�
void *ProcessDown(void *data, class FuncArg *aFuncArg)
{
	struct clidata lclidata;
	lclidata = *(struct clidata *)data;
	printf("Read socket(%d) : DownProcess : %s\n", lclidata.sockfd,
		lclidata.cbuf);
}

// quit ���ڿ��� ������ �� ȣ���� �Լ�
// ������ �����Ѵ�. 
void *ProcessQuit(void *data, class FuncArg *aFuncArg)
{
	struct clidata lclidata;
	lclidata = *(struct clidata *)data;
	printf("Read socket(%d) : QuitProcess : %s\n", lclidata.sockfd,
		lclidata.cbuf);
	close(lclidata.sockfd);
}

// ���������ڰ� ����Ÿ�� �ñ׳ο� �����ϵ��� �����Ѵ�.
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

// Ŭ���̾�Ʈ�� ���� �о���� �����͸� ó���� ������ �Լ�
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

		// �����尡 ó���ϱ�� ������ ���Ͽ��� ����Ÿ�� �ñ׳��� �߻��ϴ°� ��ٸ���.
		ret = sigwaitinfo(&sigset, &sinfo);
		if (ret == (SIGRTMIN + signum))
		{
			// ���� ������ ũ�⸦ �о�´�
			if ((readlen = read(sinfo.si_fd, (void *)&datalen, sizeof(int))) <= 0)
			{
				// ���� �����͸� �д� ���� ������ �߻��ߴٸ�, 
				// ������ �ݰ�
				// ������Ǯ�� ���� �������Ѵ�.
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
				// Ŭ���̾�Ʈ�� ���� �����͸� �о�´�.
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

				// ó���ؾ��� �޽����� ���� �Լ��� ������ ��
				// �ش� �Լ��� ȣ���Ѵ�.
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
			cout << "�Ҵ� ������ ��ȣ " << PollMapi->second.signum << endl;
			PollMap.erase(PollMapi);

			PollMap.insert(pair<int, FdPerSignum>(clientnum, fdpersignum));
		}
		else
		{
			// ����ó��
			cout << "Unknown Error : " << ret << endl;
		}
	}
}

// �� ���������� ó���� �Լ��� ����Ѵ�.
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

	int thread_num = 2;              // ������ Ǯ�� ũ��
	vector<void *(*)(void *)> thread_list;    // ������ ����Ʈ
	vector<pthread_t> threadident(thread_num);  // ������ ID ����

												// ������ �������� ������ŭ ����Ÿ�� �ñ׳��� ����Ѵ�. 
	for (loopi = 0; loopi < thread_num + 1; loopi++)
	{
		sigaddset(&sigset, SIGRTMIN + (loopi + 1));
	}

	// ��ϵ� ����Ÿ�� �ñ׳��� �߻��Ұ�� ���ǵ��� �����Ѵ�.
	sigprocmask(SIG_BLOCK, &sigset, NULL);
	// �� ���������� ó���� �Լ��� ����Ѵ�. 
	ProtocolFuncRegist();

	// Ŭ���̾�Ʈ�� ������ ��ٸ��� �����带 ����Ѵ�. 
	thread_list.push_back(socket_acceptor);

	// Ŭ���̾�Ʈ�� �����͸� ó���ϴ� �����带 ����Ѵ�.
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