#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> AL1, AL2, AL3;

    AL1.push(1);
    AL1.push(2);
    AL2.push(3);
    AL3.push(4);

    cout << "Vehicle in AL1: " << AL1.front() << endl;
    cout << "Vehicle in AL2: " << AL2.front() << endl;
    cout << "Vehicle in AL3: " << AL3.front() << endl;

    return 0;
}
