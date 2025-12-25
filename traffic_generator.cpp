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
        char road = 'A' + rand() % 4;   // Roads A, B, C, D
        int lane = 1 + rand() % 3;      // Lanes 1, 2, 3

        string filename = "lane";
        filename += road;
        filename += ".txt";

        ofstream file(filename, ios::app);
        file << vehicleId << " " << lane << endl;
        file.close();

        cout << "[GENERATOR] Vehicle " << vehicleId
             << " -> Road " << road
             << " Lane L" << lane << endl;

        vehicleId++;

        // Random arrival time (1–3 sec)
        this_thread::sleep_for(chrono::seconds(1 + rand() % 3));
    }

    return 0;
}
