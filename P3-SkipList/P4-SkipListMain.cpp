

#include <iostream>
#include <random>
#include "SkipList.h"
#include<windows.h>
#include<stdio.h>   
#include<tchar.h>

using namespace std;


int main()
{



	cout << "creating integer skip list\nenter max index: ";
	SkipList<int, int> list1;
		int max = 0;
	while (true) {
		cin >> max;
		for (int i = 0; i < max; i++) {
			list1.insert(i, i);
		}
	list1.printList();

	}

	return 0;
}
