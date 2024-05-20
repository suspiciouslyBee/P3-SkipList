

#include <iostream>
#include <random>
#include "SkipList.h"
#include<windows.h>
#include<stdio.h>   
#include<tchar.h>

using namespace std;


int main()
{



    cout << "creating integer list\n";
    SkipList<int, int> list1;

    for (int i = 0; i < 2; i++) {
        cout << "inserting " << i << ": " <<
            (list1.insert(i, i) ? "SUCCESS" : "FAILURE") << endl;
    }

    list1.printList();
    return 0;
}
