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
        char road = 'A' + rand() % 4;
        int lane = 1 + rand() % 3;

        string file = "lane";
        file += road;
        file += ".txt";

        ofstream out(file, ios::app);
        out << vehicleId << " " << road << " " << lane << endl;
        out.close();

        cout << "[GEN] Vehicle " << vehicleId
             << " -> Road " << road
             << " Lane " << lane << endl;

        vehicleId++;
        std::this_thread::sleep_for(std::chrono::seconds(1));

    }
}
