#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

int main() {
    srand(time(0));
    int vehicleId = 1;

    while (true) {
        char road = 'A' + rand() % 4;   // A, B, C, D
        int lane = 1 + rand() % 3;      // 1, 2, 3

        string filename = "lane";
        filename += road;
        filename += ".txt";

        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << vehicleId << " " << road << " " << lane << endl;
            file.close();
        }

        cout << "[GEN] Vehicle " << vehicleId
             << " -> Road " << road
             << " Lane L" << lane << endl;

        vehicleId++;

        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
