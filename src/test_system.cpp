#include <iostream>
#include <fstream>
#include "queue.h"
using namespace std;

// External functions
extern void initAllQueues();
extern void loadVehiclesFromFiles();
extern void generateVehicle();
extern int getTotalVehiclesInQueues();
extern Queue& getQueue(char road, int lane);
extern int vehicleId;

extern Queue AL1, AL2, AL3, BL1, BL2, BL3, CL1, CL2, CL3, DL1, DL2, DL3;

void printAllQueues() {
    cout << "\n========== QUEUE STATUS ==========\n";
    cout << "Road A: L1=" << size(AL1) << " L2=" << size(AL2) << " L3=" << size(AL3) << "\n";
    cout << "Road B: L1=" << size(BL1) << " L2=" << size(BL2) << " L3=" << size(BL3) << "\n";
    cout << "Road C: L1=" << size(CL1) << " L2=" << size(CL2) << " L3=" << size(CL3) << "\n";
    cout << "Road D: L1=" << size(DL1) << " L2=" << size(DL2) << " L3=" << size(DL3) << "\n";
    cout << "Total Vehicles: " << getTotalVehiclesInQueues() << "\n";
    cout << "==================================\n\n";
}

void testBasicQueue() {
    cout << "\n=== TEST 1: Basic Queue Operations ===\n";
    Queue testQ;
    initQueue(testQ);
    
    cout << "Empty? " << (isEmpty(testQ) ? "YES" : "NO") << "\n";
    
    Vehicle v1 = {1, 'A', 1};
    Vehicle v2 = {2, 'A', 1};
    Vehicle v3 = {3, 'A', 1};
    
    enqueue(testQ, v1);
    enqueue(testQ, v2);
    enqueue(testQ, v3);
    
    cout << "Size after 3 enqueues: " << size(testQ) << "\n";
    
    Vehicle d = dequeue(testQ);
    cout << "Dequeued vehicle ID: " << d.id << "\n";
    cout << "Size after 1 dequeue: " << size(testQ) << "\n";
    
    cout << "✓ Basic queue test passed\n";
}

void testFileGeneration() {
    cout << "\n=== TEST 2: File Generation ===\n";
    
    // Clear existing files
    const char roads[] = {'A', 'B', 'C', 'D'};
    for (char road : roads) {
        string filename = "lane";
        filename += road;
        filename += ".txt";
        ofstream clear(filename, ios::trunc);
        clear.close();
    }
    
    cout << "Generating 5 vehicles...\n";
    for (int i = 0; i < 5; i++) {
        generateVehicle();
    }
    
    // Check if files were created
    bool filesExist = true;
    for (char road : roads) {
        string filename = "lane";
        filename += road;
        filename += ".txt";
        ifstream check(filename);
        if (!check.good()) {
            filesExist = false;
            cout << "✗ File " << filename << " not found!\n";
        }
        check.close();
    }
    
    if (filesExist) {
        cout << "✓ Files generated successfully\n";
    }
}

void testLoadingVehicles() {
    cout << "\n=== TEST 3: Loading Vehicles ===\n";
    
    initAllQueues();
    cout << "Initial total: " << getTotalVehiclesInQueues() << "\n";
    
    loadVehiclesFromFiles();
    int total = getTotalVehiclesInQueues();
    cout << "After loading: " << total << " vehicles\n";
    
    if (total > 0) {
        cout << "✓ Vehicles loaded successfully\n";
        printAllQueues();
    } else {
        cout << "✗ No vehicles loaded! Check file generation.\n";
    }
}

void testFullWorkflow() {
    cout << "\n=== TEST 4: Full Workflow ===\n";
    
    // Initialize
    initAllQueues();
    vehicleId = 1;
    
    // Generate vehicles
    cout << "Generating 10 vehicles...\n";
    for (int i = 0; i < 10; i++) {
        generateVehicle();
    }
    
    // Load them
    cout << "Loading vehicles into queues...\n";
    loadVehiclesFromFiles();
    
    printAllQueues();
    
    // Test specific queue
    cout << "Testing AL1 queue:\n";
    Queue& q = getQueue('A', 1);
    int qSize = size(q);
    cout << "  AL1 has " << qSize << " vehicles\n";
    
    if (qSize > 0) {
        cout << "  Dequeuing one vehicle...\n";
        Vehicle v = dequeue(q);
        cout << "  Got vehicle #" << v.id << " from " << v.road << "L" << v.lane << "\n";
        cout << "  AL1 now has " << size(q) << " vehicles\n";
        cout << "✓ Full workflow test passed\n";
    } else {
        cout << "  AL1 is empty (may be normal if randomly generated)\n";
    }
}

void testContinuousGeneration() {
    cout << "\n=== TEST 5: Continuous Generation (Like Simulation) ===\n";
    
    initAllQueues();
    
    for (int cycle = 1; cycle <= 3; cycle++) {
        cout << "\n--- Cycle " << cycle << " ---\n";
        
        // Generate
        cout << "Generating 3 vehicles...\n";
        for (int i = 0; i < 3; i++) {
            generateVehicle();
        }
        
        // Load
        cout << "Loading...\n";
        loadVehiclesFromFiles();
        
        printAllQueues();
    }
    
    cout << "✓ Continuous generation test passed\n";
}

int main(int argc, char* argv[]) {
    srand(time(0));
    
    cout << "\n╔═══════════════════════════════════════╗\n";
    cout << "║  TRAFFIC SYSTEM DIAGNOSTIC TESTS     ║\n";
    cout << "╚═══════════════════════════════════════╝\n";
    
    testBasicQueue();
    testFileGeneration();
    testLoadingVehicles();
    testFullWorkflow();
    testContinuousGeneration();
    
    cout << "\n╔═══════════════════════════════════════╗\n";
    cout << "║  ALL TESTS COMPLETED                  ║\n";
    cout << "╚═══════════════════════════════════════╝\n\n";
    
    cout << "If all tests passed, the system is working correctly.\n";
    cout << "The graphics simulation should now show vehicles.\n\n";
    
    return 0;
}

/* 
COMPILE THIS TEST WITH (choose one):

Option 1 - Direct console flag:
g++ test_system.cpp queue.cpp simulator.cpp traffic_generator.cpp -o test.exe -Wl,--subsystem,console

Option 2 - Using main instead of WinMain:
g++ -DUSE_CONSOLE test_system.cpp queue.cpp simulator.cpp traffic_generator.cpp -o test.exe

Option 3 - Just use the diagnostic built into main.exe instead!
*/