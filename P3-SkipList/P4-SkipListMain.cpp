

#include <iostream>
#include <random>
#include "SkipList.h"


using namespace std;


int main()
{

	cout << "creating integer skip list\n";
	SkipList<int, int> list1;

	for (int i = 0; i < 20; i++) {
		list1.insert(i, i);
	}
	list1.displayList();

	cout << "deleting evens\n";

	for (int i = 0; i < 20; i += 2) {
		list1.remove(i);
	}
	list1.displayList();

	cout << "clearing list\n";
	list1.clearList();
	list1.displayList();
	
	cout << "reinserting integers\n";
	for (int i = 0; i < 20; i++) {
		list1.insert(i, i);
	}
	list1.displayList();
	
	cout << "\nTesting Find\n";

	for (int i = 0; i < 40; i += 6) {
		cout << "finding " << i << endl;
		if (list1.find(i)) {
			cout << "FOUND: " << list1.thisValue();
		}
		else {
			cout << "Not Found";
		}
		cout << endl << endl;
	}

	cout << "==============================\n\n";


	return 0;
}
