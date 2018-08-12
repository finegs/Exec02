
#if 0

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <initializer_list>

using namespace std;

constexpr int getArraySize() { return 32; }

int makeSum(initializer_list<int> lst)
{
	int total = 0;
	for (const auto& value : lst)
	{
		total += value;
	}
	return total;
}

int main(int argc, char** argv)
{
	string a, b;
	vector<string> myVec = { "string1", "string2", "string3" };
	cin.clear();
	cout << "Enter new word : "; cout.flush();
	cin >> a;
	cout << "Your word is : " << a << endl;

	int myArray[getArraySize()];

	int aa = makeSum({ 1,2,3,4,5 });
	cout << "sum : " << aa << endl;

	return 0;
}


#endif

#if 0
#include <iostream>
#include <cstdlib>
#include <initializer_list>
#include <sstream>
#include <algorithm>

using namespace std;

enum HeapType { Min = 1, Max};

struct Heap {
	int capacity;
	int count;
	int *ar;
	HeapType heapType;

	Heap(int capacity, HeapType heapType) : capacity(capacity), count(0), heapType(heapType) {
		if (capacity < 0) {
			throw "Heap constructor is failed : capacity is negative : " + capacity; 
		}

		ar = new int[capacity];
		if (!ar) {
			stringstream ss;
			ss << "Heap constructor is failed : new int[" << capacity << "]";
			throw ss.str();
		}
	}

	~Heap() {
		free();
	}

	void free() {
		if (!ar) return;
		delete[] ar;
		ar = nullptr;
		capacity = 0;
		count = 0;
	}

	Heap(const Heap& src) : capacity(src.capacity), count(src.count), heapType(src.heapType) {
		if (this == &src) return;
		if(!(ar = copyAr(src.ar, src.capacity))) {
			cerr << "Copy failed : " << src.capacity;
			return;
		}
	}

	Heap(Heap&& src) : capacity(src.capacity), count(src.count), heapType(src.heapType) {
		if (this == &src) return;
		if (!(ar = copyAr(src.ar, src.capacity))) {
			throw "Heap(Heap&& src) : Copy failed, capacity= " + src.capacity;
		}
		src.free();
	}

	const Heap& operator=(const Heap& src) {
		if (this == &src) return *this;
		capacity = src.capacity;
		count = src.count;
		heapType = src.heapType;
		if (!(ar = copyAr(src.ar, src.capacity))) {
			throw "operator=(const Heap& src) : Copy failed, capacity= " + src.capacity;
		}
		return *this;
	}

	Heap& operator=(Heap&& rhs) {
		if (this == &rhs) return *this;
		free();

		capacity = rhs.capacity;
		count = rhs.count;
		heapType = rhs.heapType;
		if (!(ar = copyAr(rhs.ar, rhs.capacity))) {
			throw "operator=(Heap&& rhs) : move failed, capacity= " + rhs.capacity;
		}
		return *this;
	}

	int* copyAr(int* src, int n) {
		int* dst = new int[n];
		if (dst) {
			memcpy(dst, src, sizeof(int)*n);
		}
		return dst;
	}


	int parent(int i) {
		if (i < 0 || i >= count) return -1;
		return (i - 1) / 2;
	}

	int leftChild(int i) {
		int left = i * 2 + 1;
		if (left >= count) return -1;
		return left;
	}

	int rightChild(int i) {
		int right = i * 2 + 2;
		if (right >= count) return -1;
		return right;
	}

	int getMax() {
		if (count <= 0) return -1;
		return count;
	}

	int percolateDown(int i) {
		if (i < 0 || i >= count) return -1;
		int l, r, max, temp;
		l = leftChild(i);
		r = rightChild(i);
		if (l != -1 && ar[l] > ar[i]) max = l;
		else max = i;

		if (r != -1 && ar[r] > ar[max]) max = r;

		if (max != i) {
			temp = ar[max];
			ar[max] = ar[i];
			ar[i] = temp;
			return percolateDown(max);
		}
		return -1;
	}

	int resize() {
		int oldCapacity = capacity;
		int* oldAr = ar;
		capacity *= 2;
		ar = new int[capacity];
		if (!ar) {
			capacity = oldCapacity;
			ar = oldAr;
			throw "Heap resize failed : Memory Error , capacity=" + capacity;
		}
		if (oldAr) {
			for (int i = 0; i < count; i++) ar[i] = oldAr[i];
			delete[] oldAr;
			oldAr = nullptr;
		}
		return EXIT_SUCCESS;
	}

	int insert(int data) {
		int i;
		if (count >= capacity) resize();
		count++;
		i = count - 1;
		while (i > 0 && data > ar[(i - 1) / 2]) {
			ar[i] = ar[(i - 1) / 2];
			i = (i - 1) / 2;
		}
		ar[i] = data;
		return EXIT_SUCCESS;
	}

	int deleteMax() {
		if (count <= 0) return -1;
		int data;
		data = ar[0];
		ar[0] = ar[count - 1];
		ar[count - 1] = 0;
		count--;
		percolateDown(0);
		return data;
	}

	void buildHeap(int a[], int n) {
		for (int i = 0; i < n; i++)
			insert(a[i]);
	}

	void buildHeap(initializer_list<int> lst) {
		for (const auto i : lst) {
			insert(i);
		}
	}

	friend ostream& operator<<(ostream& os, const Heap& heap) {
		int data;
		Heap cp = heap;
		os << "size = " << cp.count << ", data=[";
		while (-1 != (data = cp.deleteMax())) os << data << " ";
		os << "]" << endl;
		return os;
	}

	friend ostream& operator<<(ostream& os, const Heap* const heap) {
		os << *heap;
		return os;
	}
};

int hash(string s, int size) { 
	int seed = 151;
	unsigned long hash = 0;
	for (int i = 0; i < s.size; i++)
	{
		hash = (hash*seed) + s.at(i);
	}
	return hash%size;
}

int main(int argc, char* argv[]) {
	int size;
	int data;

	cin.clear();
	cout << "Enter Count of Heap : "; cout.flush();
	cin >> size;

	Heap* heap = new Heap(size, Max);
	string line;
	for (string line; getline(cin, line);) {
		transform(line.begin(), line.end(), line.begin(), ::tolower);

		if (line.rfind("exit") == 0)  break;
		if ("print" == line) {
			cout << heap << endl;
		}
		if ("printraw" == line) {
			cout << "size=" << heap->count << ", data=[";
			for (int i = 0; i < heap->count; i++) cout << heap->ar[i] << " ";
			cout << "]" << endl;
		}
		if (line.rfind("delete") == 0) {
			string s = line.substr(6);
			stringstream ss(s);

			ss >> data;
			if (data <= 0) continue;
			for (int i = 0; i < data; i++)
				heap->deleteMax();
		}
		if ("deleteall" == line) {
			int size = heap->count;
			for (int i = 0; i < size; i++)
				heap->deleteMax();
		}
		if (line.rfind("input") == 0) {
			string s = line.substr(5);
			stringstream ss(s);
			int data;
			while (ss>>data)
				heap->insert(data);
		}
		cin.clear();
	}

	delete heap;

	return EXIT_SUCCESS;
}

#endif

#if 0

#include <iostream>
#include <cstdlib>
#include <exception>
#include <sstream>

using namespace std;

class MyLinkedListException : exception {
	MyLinkedListException(const char* msg) : exception(msg) {}
	~MyLinkedListException() {
		exception::~exception();
	}
};

struct Node {
	explicit Node(int data) 
			: data(data)
			,prev(nullptr)
			, next(nullptr) {}
	~Node() {
		prev = next = nullptr;
	}
	int data;
	Node* prev;
	Node* next;
};

class MyQueueException : std::exception {
public:
	MyQueueException() : exception("MyQueueException") {}
	MyQueueException(const char* msg) : exception(msg) {}
};
struct Queue {
	int capacity;
	int size;
	Node* ar;
	Node* front;
	Node* rear;
	int frontIdx;
	int rearIdx;
	int count;

	Queue() : capacity(512), size(0), ar(alloc(capacity)), front(nullptr), rear(nullptr), count(0), frontIdx(0), rearIdx(0) {

		if (!ar) {
			stringstream ss;
			ss << "Queue() construct failed : Fail to alocate memory capacity(" << size << ")";
			throw MyQueueException(ss.str);
		}
	}

	Queue(int capacity) : capacity(capacity), ar(alloc(capacity)), front(nullptr), rear(nullptr), count(0) {

		if (!ar) {
			stringstream ss;
			ss << "Queue(int capacity) construct failed : Fail to alocate memory capacity(" << size << ")";
			throw MyQueueException(ss.str);
		}
	}

	Node* alloc(size_t size) {
		Node* newAr = new Node[size]{ Node(0)};
		count = 0;
		return newAr;
	}

	int isEmpty() {
		return front == NULL;
	}

	void resize(size_t newCapacity) {
		Node* oldAr;
		int oldCapacity;

		oldAr = ar;
		oldCapacity = capacity;

		ar = alloc(newCapacity);
		if (!ar) {
			stringstream ss;
			ss << "Queue resize failed : Fail to alocate memory capacity(" << newCapacity << ")";
			ar = oldAr;
			capacity = oldCapacity;
			throw MyQueueException(ss.str);
		}

		memcpy(ar, oldAr, oldCapacity*sizeof(Node));
		capacity = newCapacity;
		front = &ar[frontIdx];
		rear = &ar[rearIdx];

		delete[] oldAr;
		oldAr = nullptr;
	}

	void enqueue(int data) {
		while (count >= capacity) {
			try {
				resize(capacity*2);
			}
			catch (MyQueueException ex) {
				throw MyQueueException("enqueue failed");
			}
		}
		Node node = ar[++count];
		node.data = data;

		node.prev = rear;
		node.next = front;

		rear->next = &node;
		front->prev = &node;

		rear = &node;

	}

	int dequeue() {
		int value;
		if (isEmpty()) {
			cerr << "Queue is empty";
			value = 0;
		}
		else {
			Node* node;
			node = front;
			value = node->data;
			front - front->next;
			free(node);
			//if (front == rear) {
			//	front = rear = NULL;
			//	frontIdx = rearIdx = -1;
			//}
			//else {
			//	front = front->next;
			//	//frontIdx = (frontIdx + 1) % capacity;
			//}
		}
		return value;
	}

	friend class MyQueueException;
};
#endif

#if 0

#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {

	string name, t;
	string t2;
	string::size_type position;

	cout << "Enter your name:"; cout.flush();
	cin >> name;
	cin >> t2;
	//getline(cin, name);
	t2[0] = toupper(t2[0]);
	for (string::size_type i = 0; i < t2.length(); i++) {
		t2[i] = toupper(t2[i]);
	}
	name = name + " + " + t2;

	position = t2.find('O');

	const char* pc = "OO";
	//if (t2.find(pc) != t2.npos)
	if (t2.find_first_of("OO") != t2.npos)
		cout << "contains at least one " << pc << "!" << endl;
	else
		cout << "does not contain " << pc << endl;

	if (name == "") {
		cout << "You entered an empty string, " << "assigning default\n";
		name = "John";
	}
	else {
		cout << "Thank you, " 
			<< name 
			<< " for running this simple program!" 
			<< endl;
	}
	return EXIT_SUCCESS;
}

#endif

#if 0

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

#include <boost/regex.hpp>


int main(int argc, char* argv[]) {

	boost::regex fullname_regex("\([A-Z]+[A-Za-z]*\), \([A-Z][A-Za-z]*\)");

	string name;

	cout << "Enter you full name : "; cout.flush(); cin.clear();
	getline(cin, name);
	if (!regex_match(name, fullname_regex)) {
		cout << "Error: name not entered correctly" << endl;
	}

	return EXIT_SUCCESS;
}

#endif

#if 0

#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>

#include <vector>

using namespace std;
int main(int argc, char* argv[]) {

	string text = "hello world, this is a test";
	string fragment = text.substr(6, 5);

	cout << fragment << endl;

	cout << text.substr(text.length() - 3, 5) << endl;

	string iso_date;
	int year, month, day;
	char dash1, dash2;

	cout << "Enter date (yyyy-mm-dd): " << flush;
	getline(cin, iso_date);

	istringstream is(iso_date);
	is >> year >> dash1 >> month >> dash2 >> day;

	cout << "Your date : " << year << "-" << month << "-" << day << endl;

	vector<double> marks(20);
	for (vector<double>::size_type i = 0; i < marks.size(); i++) {
		cout << "Enter marks for #" << i + 1 << " : " << flush;
		cin >> marks[i]; cin.clear();
	}

	for (vector<double>::size_type i = 0; i < marks.size(); i++) {
		cout << "Marks[" << i << "]=" << marks[i] << endl;
	}

	int idx = 0;
	for (vector<double>::iterator i = marks.begin(); i != marks.end(); ++i) {
		cout << "#" << idx++ << "=" << *i << endl;
	}

	return EXIT_SUCCESS;
}
#endif

#if 1

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "MyList.h"

#include "MyService.h"

void mdestroy(void *data) {
	if (data == NULL) return;
	free(data);
	data = NULL;
}

void printUsage() {
	printf("Usage : add {word}\n");
	printf("		remove {n times to remove}\n");
	printf("		print\n");
}

int main(int argc, char ** argv) {

	int isRun = 1;
	List lp;
	char *line, *cmd;
	int len = 1024*1024;

	list_init(&lp, mdestroy);

	if (NULL == (line = (char*)malloc(sizeof(char) * len))) return -1;

	memset(line, '\0', len);
	while (isRun && fgets(line, len, stdin) != NULL) {
		//char* p;
		//if (p = strchr(line, '\n')) {
		//	*p = '\0';
		//}
		//else {
		//	scanf("%*[^\n]%*c");
		//}

		//memset(word, '\0', len);

		if ((cmd = strtok(line, " \n")) == NULL) {
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
			list_info(&lp, '\0');
			continue;
		}
		else if (!strcmp(cmd, "print")) {
			list_prt(&lp, '1');
			fflush(stdin);
		}
		else if (!strcmp(cmd, "print2")) {
			list_prt(&lp, '2');
			fflush(stdin);
		}
		else if (!strcmp(cmd, "remove")) {
			int i, cnt;
			char *word, *data;

			cnt = 0;
			if (NULL == (word = strtok(NULL, " \n"))) {
				cnt = 1;
			}
			
			if (word) {
				if (!strcmp("all", word)) {
					word = NULL;
					if (NULL == (word = (char*)malloc(sizeof(char) * 2))) return -1;

					memset(word, '\0', 2);
					printf("Are you sure to remove all(y/n)? (size=%d)", (int)list_size(&lp));

					scanf("%s", word);
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

						scanf("%1s", word);
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
			while ((data = strtok(NULL, " \n")) != NULL) {
				size_t dataLen;
				newData = (char*)malloc(dataLen = (sizeof(char)*strlen(data)+1));
				memcpy(newData, data, dataLen-1);
				memset(newData+dataLen-1, '\0', 1);
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
			if ((data = strtok(NULL, " \n")) == NULL) {
				fflush(stdin);
				continue;
			}
			else if ((n = atoi(data)) < 1) {
				fflush(stdin);
				continue;
			}
	
			while (isRun && (data = strtok(NULL, " \n")) != NULL) {
				for (int i = 0; i < n; i++) {
					size_t dataLen;
					if (NULL == (newData = (char*)malloc(dataLen = (sizeof(char)*strlen(data) + 1)))) {
						isRun = 0;
						break;
					}
					memcpy(newData, data, dataLen - 1);
					memset(newData+dataLen-1, '\0', 1);
					if (0 > list_ins_next(&lp, NULL, newData)) {
						free(newData);
						newData = NULL;
						isRun = 0;
						break;
					}
				}
			}
		}
		else if (!strcmp(cmd, "udp")) {
			char *data;
			char *newData;
			List al;

			if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }
			
			if (!strcmp("-server", data)) {
				if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

				char *ip;
				int port;

				ip = NULL;
				port = -1;

				if (!strcmp("start", data)) {
					if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

					if (!strcmp("-port", data)) {
						if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

						int port = atoi(data);
						if (port < 1 || port > 65535) {
							continue;
						}

						UDPService* udpSvc = (UDPService*)malloc(sizeof(UDPService));
						udpSvc->ip = "0.0.0.0";
						udpSvc->port = port;

						startUDPService(udpSvc);

					}
				}
				else if (!strcmp("stop", data)) {
					if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

					if (!strcmp("-port", data)) {
						if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

						int port = atoi(data);
						if (port < 1 || port > 65535) {
							continue;
						}


						UDPService* udpSvc = (UDPService*)malloc(sizeof(UDPService));
						udpSvc->ip = "0.0.0.0";
						udpSvc->port = port;

						stopUDPService(udpSvc);
					}
				}
				else {
					printf("Unsupported action : %s\n", data);
					data = NULL;
					continue;
				}
			}
			else if (!strcmp("-client", data)) {
				if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

				char *ip;
				int port;
				size_t slen;

				ip = NULL;
				port = -1;
				if (!strcmp("connect", data)) {
					if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

					if (!strcmp("-ip", data)) {
						if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }
						
						if (NULL == (ip = (char*)malloc(slen = sizeof(char)*strlen(data) + 1))) continue;

						memset(ip, '\0', slen);
						memcpy(ip, data, slen-1);
					}

					if (!strcmp("-port", data)) {
						if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

						port = atoi(data);
					}

					if (NULL == ip || port < 1 || port > 65535) {
						printf("IP is NULL or Unsupported UDP port : %d", port);
						continue;
					}

					connectUDPServer(ip, port);
				}
				else if (!strcmp("disconnect", data)) {
					if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

					char *ip;
					int port;
					size_t slen;

					ip = NULL;
					if (!strcmp("-ip", data)) {
						if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

						if (NULL == (ip = (char*)malloc(slen = sizeof(char)*strlen(data) + 1))) continue;

						memset(ip, '\0', slen);
						memcpy(ip, data, slen - 1);
					}

					if (!strcmp("-port", data)) {
						if (NULL == (data = strtok(NULL, " \n"))) { data = NULL;  continue; }

						port = atoi(data);
					}

					if (NULL == ip || port < 1 || port > 65535) {
						printf("IP is NULL or Unsupported UDP port : %d", port);
						continue;
					}

					disconnectUDPServer(ip, port);
				}
				else {
					printf("Unsupported action : %s\n", data);
					data = NULL;
					continue;
				}
			}

			while ((data = strtok(NULL, " \n")) != NULL) {
				size_t dataLen;
				newData = (char*)malloc(dataLen = (sizeof(char)*strlen(data) + 1));
				memcpy(newData, data, dataLen - 1);
				memset(newData + dataLen - 1, '\0', 1);
				if (0 > list_ins_next(&al, NULL, newData)) {
					isRun = 0;
					break;
				}
			}


		}
		else if (!strcmp(cmd, "tcp")) {
		}
		else {
			printUsage();
		}

		memset(line, '\0', len);
		//scanf("%*[^\n]%*c");
		//scanf("%*[^\n]");
	}

	free(line);
	line = NULL;
	cmd = NULL;

	list_destroy(&lp);

	return EXIT_SUCCESS;
}

#endif