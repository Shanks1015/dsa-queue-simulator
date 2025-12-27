#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "queue.h"

using namespace std;

int main() {
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

        ofstream clear("laneA.txt", ios::trunc);
        clear.close();

        // PRIORITY LANE (AL2)
        if (size(AL2) > 10) {
            cout << "[PRIORITY] Serving AL2\n";
            while (size(AL2) > 5) {
                Vehicle v = dequeue(AL2);
                cout << "Serving Vehicle " << v.id << " (AL2)\n";
                this_thread::sleep_for(chrono::seconds(1));
            }
        }

        if (!isEmpty(AL1)) {
            Vehicle v = dequeue(AL1);
            cout << "Serving Vehicle " << v.id << " (AL1)\n";
        }

        if (!isEmpty(AL3)) {
            Vehicle v = dequeue(AL3);
            cout << "Serving Vehicle " << v.id << " (AL3)\n";
        }

        this_thread::sleep_for(chrono::seconds(2));
    }
}
