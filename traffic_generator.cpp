#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

int main() {
    srand(time(0));
    int vehicleId = 1;

    while (true) {
        char road = 'A' + rand() % 4;
        int lane = 1 + rand() % 3;

        string filename = "lane";
        filename += road;
        filename += ".txt";

        ofstream file(filename.c_str(), ios::app);
        file << vehicleId << " " << lane << endl;
        file.close();

        cout << "[GEN] Vehicle " << vehicleId
             << " -> Road " << road
             << " Lane L" << lane << endl;

        vehicleId++;
        sleep(1 + rand() % 3);
    }

    return 0;
}
