#include <iostream>
#include <fstream>
#include "queue.h"
using namespace std;

// All 12 lane queues (4 roads × 3 lanes)
Queue AL1, AL2, AL3;  // Road A lanes
Queue BL1, BL2, BL3;  // Road B lanes
Queue CL1, CL2, CL3;  // Road C lanes
Queue DL1, DL2, DL3;  // Road D lanes

// Initialize all queues
void initAllQueues() {
    initQueue(AL1); initQueue(AL2); initQueue(AL3);
    initQueue(BL1); initQueue(BL2); initQueue(BL3);
    initQueue(CL1); initQueue(CL2); initQueue(CL3);
    initQueue(DL1); initQueue(DL2); initQueue(DL3);
}

// Get queue by road and lane
Queue& getQueue(char road, int lane) {
    if (road == 'A') {
        if (lane == 1) return AL1;
        if (lane == 2) return AL2;
        return AL3;
    } else if (road == 'B') {
        if (lane == 1) return BL1;
        if (lane == 2) return BL2;
        return BL3;
    } else if (road == 'C') {
        if (lane == 1) return CL1;
        if (lane == 2) return CL2;
        return CL3;
    } else {
        if (lane == 1) return DL1;
        if (lane == 2) return DL2;
        return DL3;
    }
}

// Load vehicles from files
void loadVehiclesFromFiles() {
    const char roads[] = {'A', 'B', 'C', 'D'};
    
    for (char road : roads) {
        string filename = "lane";
        filename += road;
        filename += ".txt";
        
        ifstream file(filename);
        if (!file.is_open()) continue;
        
        int id, lane;
        char r;
        while (file >> id >> r >> lane) {
            Vehicle v = {id, r, lane};
            Queue& q = getQueue(r, lane);
            enqueue(q, v);
        }
        file.close();
        
        // Clear file after loading
        ofstream clear(filename, ios::trunc);
        clear.close();
    }
}
