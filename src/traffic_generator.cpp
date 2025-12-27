#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

int vehicleId = 1;

// Generate vehicle on random road and lane
void generateVehicle() {
    // Random road: A, B, C, or D
    char road = 'A' + rand() % 4;
    
    // Random lane: 1, 2, or 3
    int lane = 1 + rand() % 3;
    
    // Write to corresponding file
    string file = "lane";
    file += road;
    file += ".txt";
    
    ofstream out(file, ios::app);
    out << vehicleId << " " << road << " " << lane << endl;
    out.close();
    
    cout << "[GEN] Vehicle #" << vehicleId 
         << " -> Road " << road 
         << " Lane " << lane << endl;
    
    vehicleId++;
}
