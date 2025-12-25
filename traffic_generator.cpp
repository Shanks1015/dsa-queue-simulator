#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {
    srand(time(0));

    ofstream fileA("laneA.txt", ios::app);
    int vehicles = rand() % 5 + 1;

    for (int i = 0; i < vehicles; i++) {
        fileA << "V\n";
    }

    fileA.close();
    return 0;
}
