
/*
* File: Main
*
* Programmer: June
*
* Description: Runs a test case of Skiplist.h with a fake large data set, and
* manipulation thereof. 
*
* This is quick, dirty, and not very optimal... but it works...
*
*/
#include <iostream>
#include <random>
#include "SkipList.h"

using namespace std;

int main()
{

	cout << "creating integer skip list\n";

	SkipList<int, int> list1;
	SkipList<int, int> list2;

	cout << "small list:\n\n";

	for (int i = 0; i < 21; i++) {
		list2.insert(i, i);
	}
	list2.displayList();


	cout << "big list:\n\n";
	for (int i = 0; i < 438; i++) {
		list1.insert(i, i);
	}
	list1.displayList();

	cout << "deleting increments of 10s\n";

	for (int i = 0; i < 438; i += 10) {
		list1.remove(i);
	}
	list1.displayList();

	cout << "\nTesting Find, increments of 25...\n";
	for (int i = 0; i < 438; i += 25) {
		cout << "finding " << i << endl;
		if (list1.find(i)) {
			cout << "FOUND: " << list1.thisValue();
		}
		else {
			cout << "Not Found";
		}
		cout << endl << endl;
	}

	cout << "clearing list\n";
	list1.clearList();
	list1.displayList();
	
	cout << "reinserting integers. (testing initing new seed)\n";
	for (int i = 0; i < 438; i++) {
		list1.insert(i, i);
	}
	list1.displayList();
	

	cout << "==============================\n\n";


	return 0;
}
