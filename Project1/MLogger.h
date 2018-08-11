#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

#ifdef __cplusplus

#include <ctime>
#include <sys/timeb.h>

extern "C" {
#endif

char* nowTime(char* str);
char* t2s(char* str, timeb tm);

#ifdef __cplusplus
}
#endif

enum MLogLevel {
	Error
	, Warning
	, Info
	, Debug
	, Debug1
	, Debug2
	, Debug3
	, Debug4
};

class MLogMsg {
	friend class MLog;
private:
	timeb* tm;
	char* msg;
public:
	explicit MLogMsg(const char* _msg);
	~MLogMsg();
	MLogMsg(const MLogMsg& other);
	void clearMsg();
	MLogMsg& operator=(const MLogMsg& msg);
	MLogMsg(MLogMsg&& other);
	MLogMsg& operator=(MLogMsg&& other);
	friend std::ostream& operator<<(std::ostream& os, const MLogMsg& logmsg);

	timeb* getTime() const;
	char* getMsg() const;

};

//template<class T>
class MLog {

	//friend class ostream;
	using endl_type = std::ostream&(std::ostream&); //This is the key: std::endl is a template function, and this is the signature of that function (For std::ostream).


public:
	MLog();
	MLog(const MLog&);
	explicit MLog(MLogLevel ll);
	virtual ~MLog();
	std::ostringstream& get(MLogLevel level);
	MLogLevel& LogLevel() {
		return logLevel;
	}
	void flush();
	void clear();
	void init();
	void pause();
	void resume();
	void start();
	void shutdown();

	MLog& operator<<(endl_type endl);

	template<typename T>
	MLog& operator<<(const T& obj);

protected:
	std::ostringstream os;

private:
	MLog& operator=(const MLog&);
	MLogLevel logLevel;

	std::atomic_bool isRun;
	std::atomic_bool isPause;
	std::condition_variable cv;
	std::priority_queue<MLogMsg*> queue;
	std::mutex mtx;
	std::thread* logWriter;

	void handleLog();
	void log(const char* _msg);
};