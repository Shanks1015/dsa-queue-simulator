#include <iostream>
#include <queue>
#include <fstream>
#include <thread>
#include <chrono>
using namespace std;

struct Vehicle {
    int id;
    char road;
    int lane;
};
queue<Vehicle> roadA, roadB, roadC, roadD;

void readLane(char road, queue<Vehicle>& q) {
    string filename = "lane";
    filename += road;
    filename += ".txt";

    ifstream file(filename);
    int id, lane;

    while (file >> id >> lane) {
        Vehicle v = {id, road, lane};
        q.push(v);
    }

    file.close();
    ofstream clearFile(filename, ios::trunc);
    clearFile.close();
}
void serve(queue<Vehicle>& q, char road) {
    if (!q.empty()) {
        cout << "Road " << road
             << " serving Vehicle " << q.front().id << endl;
        q.pop();
    }
}
bool isPriority(queue<Vehicle>& q) {
    return q.size() > 10;
}
int main() {
    while (true) {
        readLane('A', roadA);
        readLane('B', roadB);
        readLane('C', roadC);
        readLane('D', roadD);

        // Priority condition for AL2
        if (isPriority(roadA)) {
            cout << "PRIORITY MODE: Road A\n";
            while (roadA.size() > 5) {
                serve(roadA, 'A');
                this_thread::sleep_for(chrono::seconds(1));
            }
        }

        // Normal condition (round robin)
        serve(roadA, 'A');
        serve(roadB, 'B');
        serve(roadC, 'C');
        serve(roadD, 'D');

        cout << "---------------------\n";
        this_thread::sleep_for(chrono::seconds(2));
    }

    return 0;
}
