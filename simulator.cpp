#include <iostream>
#include <fstream>
#include <unistd.h>
#include "queue.h"

using namespace std;

/* Lane Queues (only L2 lanes are controlled) */
Queue AL2, BL2, CL2, DL2;

/* Read vehicles from file */
void readLane(char road, Queue &q) {
    string filename = "lane";
    filename += road;
    filename += ".txt";

    ifstream file(filename.c_str());
    int id, lane;

    while (file >> id >> lane) {
        if (lane == 2) {
            Vehicle v = {id};
            enqueue(q, v);
        }
    }
    file.close();

    ofstream clearFile(filename.c_str(), ios::trunc);
    clearFile.close();
}

/* Serve vehicles */
void serve(Queue &q, char road, int count) {
    for (int i = 0; i < count && !isEmpty(q); i++) {
        Vehicle v = dequeue(q);
        cout << "[GREEN] Road " << road
             << " serving Vehicle " << v.id << endl;
        sleep(1);
    }
}

int main() {
    initQueue(AL2);
    initQueue(BL2);
    initQueue(CL2);
    initQueue(DL2);

    while (true) {
        readLane('A', AL2);
        readLane('B', BL2);
        readLane('C', CL2);
        readLane('D', DL2);

        cout << "\n===== TRAFFIC CYCLE START =====\n";

        /* PRIORITY CONDITION */
        if (size(AL2) > 10) {
            cout << "[PRIORITY] AL2 ACTIVE\n";
            while (size(AL2) > 5) {
                serve(AL2, 'A', 1);
            }
        }
        else {
            /* NORMAL CONDITION */
            int total = size(BL2) + size(CL2) + size(DL2);
            int V = total / 3;
            if (V == 0) V = 1;

            cout << "[NORMAL] Vehicles per lane = " << V << endl;

            cout << "Light GREEN -> Road B\n";
            serve(BL2, 'B', V);

            cout << "Light GREEN -> Road C\n";
            serve(CL2, 'C', V);

            cout << "Light GREEN -> Road D\n";
            serve(DL2, 'D', V);
        }

        cout << "===== TRAFFIC CYCLE END =====\n";
        sleep(2);
    }

    return 0;
}
