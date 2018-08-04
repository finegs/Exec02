#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <sys/timeb.h>
#include <chrono>
#include "MLogger.h"

using namespace std;

char* timestamp(char* str) {
	char t[24];
	struct tm timeinfo;
	struct timeb tm;
	
	ftime(&tm);
	localtime_s(&timeinfo, &tm.time);

	strftime(t, 20, "%Y-%m-%d %H:%M:%S", &timeinfo);
	sprintf_s(str, 24, "%s.%03u", t, tm.millitm);
	return str;
}

//int main(int argc, char* argv[]) {
//	//bool isRun = true;
//	//string str;
//	//stringstream ss;
//	//char* ts = new char[24];
//
//	//MLog logger;
//
//	//logger.start();
//
//	//while (isRun)
//	//{
//	//	logger << timestamp(ts) << " input : "; logger.flush();
//	//	getline(cin, str);
//	//	ss.clear();
//	//	ss << str;
//	//	cin.clear();
//	//	logger << timestamp(ts) << " read : ";
//	//	while (ss >> str) {
//	//		logger << str <<  (ss.eof() ? "" : ", ");
//	//		transform(str.begin(), str.end(), str.begin(), tolower);
//	//		if ("exit" == str) {
//	//			logger.flush();
//	//			cin.clear();
//	//			logger.flush();
//	//			logger << "Are you sure to exit ? (Y/N) "; logger.flush();
//	//			cin >> str;
//	//			transform(str.begin(), str.end(), str.begin(), tolower);
//	//			if ("y" == str) {
//	//				isRun = false;
//	//				break;
//	//			}
//	//		}
//	//		else if ("clear" == str || "cls" == str) {
//	//			system("cls");
//	//		}
//	//	}
//	//	//logger << std::endl;
//	//	logger.flush();
//	//}
//
//	//delete[] ts;
//
//	return EXIT_SUCCESS;
//}