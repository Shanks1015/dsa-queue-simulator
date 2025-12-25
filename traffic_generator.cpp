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
        file << vehicleId << " " << lane << endl;
        file.close();

        cout << "Generated Vehicle " << vehicleId
             << " on Road " << road
             << " Lane " << lane << endl;

        vehicleId++;

        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
