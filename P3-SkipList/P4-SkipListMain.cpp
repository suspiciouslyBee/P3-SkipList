

#include <iostream>
#include <random>

using namespace std;


int main()
{
    int heads = 0;
    int tails = 0;
    int adding = 0;
    int tries = 0;
    int i = 0;

    cout << "Hello World!\n";
    while (true) {
        cout << "Enter # of tries:\n";
        cin >> tries;
        cout << "This should be a good distribution\n";

        for (i = 0; i <= tries; i++) {
            if ((rand() & 1)) {
                heads++;
            }
            else {
                tails++;
            }
        }

        cout << "Heads: " << heads << endl;
        cout << "Tails: " << tails << endl;
        heads = 0;
        tails = 0;
    }

    



}
