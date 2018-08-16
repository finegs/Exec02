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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>

//#include "..\MyUtil\MyUtil.h"
#include <ctime>
#include <sys/timeb.h>
#include "MLogger.h"

#include "MyList.h"
#include "..\MyUtil\MySort.h"

static char* trim(char* str) {
    int len = 0;
    if ((len = (int)strlen(str)) > 1
        && '\n' == str[len-1]) {
        str[len - 1] = '\0';
    }
    return str;
}

static char* nowTime(char* str) {
    char t[24];
    struct tm timeinfo;
    struct timeb tm;

    memset(t, '\0', sizeof(char) * 24);

    ftime(&tm);
    localtime_s(&timeinfo, &tm.time);

    strftime(t, 20, "%Y-%m-%d %H:%M:%S", &timeinfo);
    sprintf_s(str, 24, "%s.%03u", t, tm.millitm);
    return str;
}

int* getPi(char* p) {
    int m = (int)strlen(p), j = 0;
    int* pi;
    if (NULL == (pi = (int*)malloc(sizeof(int)*m))) {
        fprintf(stderr, "getPi failed : malloc failed, Size=%d\n", m);
        return NULL;
    }
    memset(pi, 0, sizeof(int)*m);
    for (int i = 1; i < m; i++) {
        while (j > 0 && p[i] != p[j]) j = pi[j - 1];
        if (p[i] == p[j]) pi[i] = ++j;
    }
    return pi;
}

int* kmp(char* s, char* p, int* ansi) {
    int* ans;
    int* pi = getPi(p);
    int n = (int)strlen(s), m = (int)strlen(p), j = 0;

    if (NULL == (ans = (int*)malloc(sizeof(int)*(n))))
        return NULL;

    memset(ans, -1, sizeof(int)*n); // init ans array with -1 (no setting)
    *ansi = 0;

    for (int i = 0; i < n; i++) {
        while (j > 0 && s[i] != p[j]) j = pi[j - 1];
        if (s[i] == p[j]) {
            if (j == m - 1) {
                ans[*ansi] = i - m + 1;
                j = pi[j];
                (*ansi)++;
            }
            else {
                j++;
            }
        }
    }

    if (pi) {
        free(pi);
        pi = NULL;
    }

    return ans;
}

int intslen(void* ar, int(isValue)(void* vp), int(isEnd)(void* vp)) {
    int i, cnt;
    cnt = 0;
    for (i = 0; !isEnd(((int*)ar)+i); i++) 
        if(isValue(((int*)ar)+i)) cnt++;
    return cnt;
}
 
int isIntArrEnd(void* vp) {
    return *((int*)vp) < 0;
}

int isValueSet(void* vp) {
    return *((int*)vp) > 0;
}

void printUsage() {
    printf("Usage : add {word}\n");
    printf("		remove {n times to remove}\n");
    printf("		print\n");
}


int main(int argc, char* argv[]) {

#if 0
    static int AR_SIZE = 10;

    int* ar;

    ar = (int*)malloc(sizeof(int) * AR_SIZE);

    memset(ar, 0x00, sizeof(int)*AR_SIZE);

    for (int i = 0; i < AR_SIZE; i++) {
        printf("% 10d : ar[%d] = %d\n", i, i, *((int*)ar + i));
        printf("% 10d : ar[%d] = %d\n", i, i, ar[i]);
    }

#endif // 0

#if 0
    int LINE_SIZE = 1024 * 1024;
    char* line, *pattern;
    char* ts;

    if (NULL == (ts = (char*)malloc(sizeof(char) * 20))) {
        fprintf(stderr, "timestamp malloc failed : %d\n", 20);
        return EXIT_FAILURE;
    }

    if (NULL == (line = (char*)malloc(LINE_SIZE))) {
        fprintf(stderr, "%s : line malloc failed : %d\n", nowTime(ts), LINE_SIZE);
        return EXIT_FAILURE;
    }
    if (NULL == (pattern = (char*)malloc(LINE_SIZE))) {
        fprintf(stderr, "%s : pattern malloc failed : %d\n", nowTime(ts), LINE_SIZE);
        return EXIT_FAILURE;
    }

    memset(ts, '\0', sizeof(char)*20);
    memset(line, '\0', sizeof(char)*LINE_SIZE);
    memset(pattern, '\0', sizeof(char)*LINE_SIZE);


    printf("%s : Eneter String : ", nowTime(ts)); fflush(stdout);
    if (NULL == (fgets(line, LINE_SIZE, stdin))) {
        return EXIT_SUCCESS;
    }
    trim(line);
    printf("%s : Eneter Pattern : ", nowTime(ts)); fflush(stdout);
    if (NULL == (fgets(pattern, LINE_SIZE, stdin))) {
        return EXIT_SUCCESS;
    }
    trim(pattern);

    printf("%s : String : %s, Pattern : %s\n", nowTime(ts), line, pattern);
    int ansi;
    int* ans = kmp(line, pattern, &ansi);
    int cnt = 0;
    printf("%s : ANSI = %d, Match Count : %d\n", nowTime(ts), ansi, cnt = intslen(ans, isValueSet, isIntArrEnd));
    for (int i = 0; i < ansi; i++)
    {
        printf("  : ANS[%d] = %d\n", i, *(ans+i));
    }

    free(ans);
    free(line);
    free(pattern);

    ans = NULL, line = NULL, pattern = NULL;
#endif // 0

#if 1
    int LINE_SIZE = 1024 * 1024;
    int isRun;
    char *line, *cmd, *pattern;
    char* ts;
    List lp;
    char* context = NULL;

    if (NULL == (ts = (char*)malloc(sizeof(char) * 20))) {
        fprintf(stderr, "timestamp malloc failed : %d\n", 20);
        return EXIT_FAILURE;
    }

    if (NULL == (line = (char*)malloc(sizeof(char) * LINE_SIZE))) {
        fprintf(stderr, "line buffer malloc failed : %d\n", LINE_SIZE);
        if (ts) free(ts);
        return EXIT_FAILURE;
    }

    isRun = 1; // Init : isRun(1) = run
    memset(line, '\0', LINE_SIZE);
    list_init(&lp, list_delement_destroy);

    while (isRun && fgets(line, LINE_SIZE, stdin) != NULL) {

        if ((cmd = strtok_s(line, " \n", &context)) == NULL) {
            continue;
        }

        if (!strcmp(cmd, "exit")) {
            isRun = 0;
            break;
        }
        else if (!strcmp(cmd, "cls") || !strcmp(cmd, "clear")) {
            system("cls");
            continue;
        }
        else if (!strcmp(cmd, "size") || !strcmp(cmd, "info")) {
            //list_info(&lp, '\0');
            continue;
        }
        else if (!strcmp(cmd, "print")) {
            //list_prt(&lp, '1');
            fflush(stdin);
        }
        else if (!strcmp(cmd, "print2")) {
            //list_prt(&lp, '2');
            fflush(stdin);
        }
        else if (!strcmp(cmd, "remove")) {
            int i, cnt;
            char *word, *data;

            cnt = 0;
            if (NULL == (word = strtok_s(NULL, " \n", &context))) {
                cnt = 1;
            }

            if (word) {
                if (!strcmp("all", word)) {
                    word = NULL;
                    if (NULL == (word = (char*)malloc(sizeof(char) * 2))) return -1;

                    memset(word, '\0', 2);
                    printf("Are you sure to remove all(y/n)? (size=%d)", (int)list_size(&lp));

                    scanf_s("%s", word);
                    if (!strcmp("y", word) || !strcmp("Y", word)) {
                        cnt = list_size(&lp);
                    }

                    free(word);
                    word = NULL;
                }
                else {
                    cnt = atoi(word);
                    if (cnt > 0) {
                        word = NULL;
                        if (NULL == (word = (char*)malloc(sizeof(char) * 2))) {
                            isRun = 0;
                            break;
                        }
                        memset(word, '\0', 2);

                        printf("Are you sure to remove %d words(y/n)?", cnt);

                        scanf_s("%1s", word);
                        fflush(stdin);
                        if (strcmp("y", word) && strcmp("Y", word)) {
                            cnt = 0;
                        }

                        free(word);
                        word = NULL;
                    }
                }
            }

            if (cnt > list_size(&lp)) continue;

            for (i = 0; i < cnt; i++) {
                list_rem_next(&lp, NULL, (void**)&data);
                printf("list[%i]=%s is removed\n", i, data);
                free(data);
                data = NULL;
            }
        }
        else if (!strcmp(cmd, "add")) {
            char *data;
            char *newData;
            while ((data = strtok_s(NULL, " \n", &context)) != NULL) {
                size_t dataLen;
                newData = (char*)malloc(dataLen = (sizeof(char)*strlen(data) + 1));
                memcpy(newData, data, dataLen - 1);
                memset(newData + dataLen - 1, '\0', 1);
                if (0 > list_ins_next(&lp, NULL, newData)) {
                    isRun = 0;
                    break;
                }
            }
        }
        else if (!strcmp(cmd, "madd")) {
            int n;
            char *data;
            char *newData;
            if ((data = strtok_s(NULL, " \n", &context)) == NULL) {
                fflush(stdin);
                continue;
            }
            else if ((n = atoi(data)) < 1) {
                fflush(stdin);
                continue;
            }

            while (isRun && (data = strtok_s(NULL, " \n", &context)) != NULL) {
                for (int i = 0; i < n; i++) {
                    size_t dataLen;
                    if (NULL == (newData = (char*)malloc(dataLen = (sizeof(char)*strlen(data) + 1)))) {
                        isRun = 0;
                        break;
                    }
                    memcpy(newData, data, dataLen - 1);
                    memset(newData + dataLen - 1, '\0', 1);
                    if (0 > list_ins_next(&lp, NULL, newData)) {
                        free(newData);
                        newData = NULL;
                        isRun = 0;
                        break;
                    }
                }
            }
        }
        else {
            printUsage();
        }

        memset(line, '\0', LINE_SIZE);
        //scanf("%*[^\n]%*c");
        //scanf("%*[^\n]");
    }


    return EXIT_SUCCESS;

#endif


}