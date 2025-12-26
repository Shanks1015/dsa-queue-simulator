#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "queue.h"

using namespace std;

int main() {

    // Queues for Road A lanes
    Queue AL1, AL2, AL3;
    initQueue(AL1);
    initQueue(AL2);
    initQueue(AL3);

    while (true) {
        ifstream file("laneA.txt");
        int id, lane;
        char road;

        while (file >> id >> road >> lane) {
            Vehicle v = {id, road, lane};

            if (lane == 1) enqueue(AL1, v);
            else if (lane == 2) enqueue(AL2, v);
            else enqueue(AL3, v);
        }
        file.close();

        // Clear file after reading
        ofstream clearFile("laneA.txt", ios::trunc);
        clearFile.close();

        // 🔴 PRIORITY LANE LOGIC (AL2)
        if (size(AL2) > 10) {
            cout << "[PRIORITY] Serving AL2" << endl;
            while (size(AL2) > 5) {
                Vehicle v = dequeue(AL2);
                cout << "Serving Vehicle " << v.id << " (AL2)" << endl;
                std::this_thread::sleep_for(chrono::seconds(1));
            }
        }

        // Normal serving
        if (!isEmpty(AL1)) {
            Vehicle v = dequeue(AL1);
            cout << "Serving Vehicle " << v.id << " from AL1" << endl;
        }

        if (!isEmpty(AL3)) {
            Vehicle v = dequeue(AL3);
            cout << "Serving Vehicle " << v.id << " from AL3" << endl;
        }

        std::this_thread::sleep_for(chrono::seconds(2));
    }

    return 0;
}
