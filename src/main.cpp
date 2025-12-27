#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<int> AL1, AL2, AL3;

   
    AL1.push(1);
    AL2.push(2);
    AL2.push(3);
    AL3.push(4);

    cout << "AL1 front: " << AL1.front() << endl;
    cout << "AL2 front: " << AL2.front() << endl;
    cout << "AL3 front: " << AL3.front() << endl;

    return 0;
}
