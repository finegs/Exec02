
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

class NamedLinkedList {
public:
	NamedLinkedList(string& _name) : sName(_name), pNext(NULL) {}
	void add() {
		this->pNext = pHead;
		pHead = this;
	}

	static NamedLinkedList* first() { return pHead; }
	NamedLinkedList* next() { return pNext; }
	const string& name() { return sName; }
	
protected:
	string sName;

	static NamedLinkedList* pHead;
	NamedLinkedList* pNext;
};

NamedLinkedList* NamedLinkedList::pHead = NULL;

NamedLinkedList* getData() {
	string name;
	cout << "Enter name:";
	cin >> name;
	if (name == "exit") {
		return 0;
	}
	return new NamedLinkedList(name);
}

int main(int argc, char* argv[]) {
	cout << "Enter 'exit' for first name to exit" << endl;

	NamedLinkedList* pNDS;
	while (pNDS = getData()) {
		pNDS->add();
	}

	cout << "\nEntries:" << endl;

	for (NamedLinkedList *pIter = NamedLinkedList::first(); pIter; pIter = pIter->next()) {
		cout << pIter->name() << (pIter->next() ? ", " : "");
	}
	cout << endl;
	return EXIT_SUCCESS;
}