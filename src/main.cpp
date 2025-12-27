#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include "queue.h"

using namespace std;

Queue AL1, AL2, AL3, BL1, BL2, BL3, CL1, CL2, CL3, DL1, DL2, DL3;
int vehicleId = 1;

void initAllQueues() {
    initQueue(AL1); initQueue(AL2); initQueue(AL3);
    initQueue(BL1); initQueue(BL2); initQueue(BL3);
    initQueue(CL1); initQueue(CL2); initQueue(CL3);
    initQueue(DL1); initQueue(DL2); initQueue(DL3);
}

Queue& getQueue(char road, int lane) {
    if (road == 'A') return (lane == 1) ? AL1 : (lane == 2) ? AL2 : AL3;
    if (road == 'B') return (lane == 1) ? BL1 : (lane == 2) ? BL2 : BL3;
    if (road == 'C') return (lane == 1) ? CL1 : (lane == 2) ? CL2 : CL3;
    return (lane == 1) ? DL1 : (lane == 2) ? DL2 : DL3;
}

void loadVehiclesFromFiles() {
    const char roads[] = {'A', 'B', 'C', 'D'};
    
    for (char road : roads) {
        for (int lane = 1; lane <= 3; lane++) {
            string filename = "lane_" + string(1, road) + to_string(lane) + ".txt";
            ifstream file(filename);
            
            if (file.is_open()) {
                int id, laneNum;
                char roadChar;
                
                vector<Vehicle> tempVehicles;
                while (file >> id >> roadChar >> laneNum) {
                    Vehicle v = {id, roadChar, laneNum};
                    tempVehicles.push_back(v);
                }
                file.close();
                
                for (const auto& v : tempVehicles) {
                    Queue& q = getQueue(v.road, v.lane);
                    if (!isFull(q)) {
                        enqueue(q, v);
                    }
                }
                
                if (!tempVehicles.empty()) {
                    ofstream clearFile(filename, ios::trunc);
                    clearFile.close();
                }
            }
        }
    }
}

void generateVehicle() {
    const char roads[] = {'A', 'B', 'C', 'D'};
    char road = roads[rand() % 4];
    int lane = (rand() % 3) + 1;
    
    string filename = "lane_" + string(1, road) + to_string(lane) + ".txt";
    ofstream file(filename, ios::app);
    if (file.is_open()) {
        file << vehicleId << " " << road << " " << lane << endl;
        file.close();
    }
    vehicleId++;
}

const int WINDOW_WIDTH = 1800;
const int WINDOW_HEIGHT = 1000;
const int ROAD_WIDTH = 300;
const int LANE_WIDTH = 100;
const int JUNCTION_SIZE = 300;

SDL_Color COLOR_BG = {15, 23, 42, 255};
SDL_Color COLOR_ROAD = {55, 65, 81, 255};
SDL_Color COLOR_JUNCTION = {30, 41, 59, 255};
SDL_Color COLOR_LANE_LINE = {251, 191, 36, 255};
SDL_Color COLOR_LANE1 = {59, 130, 246, 255};
SDL_Color COLOR_LANE2 = {251, 146, 60, 255};
SDL_Color COLOR_LANE3 = {34, 197, 94, 255};
SDL_Color COLOR_TEXT = {241, 245, 249, 255};
SDL_Color COLOR_GREEN = {34, 197, 94, 255};
SDL_Color COLOR_RED = {239, 68, 68, 255};
SDL_Color COLOR_YELLOW = {251, 191, 36, 255};

enum TurnDirection { STRAIGHT, LEFT };

struct AnimatedVehicle {
    int id;
    char road;
    int lane;
    float x, y;
    vector<pair<float, float>> waypoints;
    int currentWaypoint;
    bool isMoving;
    SDL_Color color;
    float speed;
    TurnDirection direction;
    float delayTimer; // NEW: Stagger vehicle release
};

vector<AnimatedVehicle> movingVehicles;

enum LightState { RED = 0, GREEN = 1 };

struct TrafficLight {
    LightState state;
    char road;
};

TrafficLight lights[4] = {{GREEN, 'A'}, {RED, 'B'}, {RED, 'C'}, {RED, 'D'}};
int currentGreenLight = 0;

bool needsPriority() {
    return size(AL2) > 10;
}

int calculateVehiclesToServe() {
    int totalVehicles = 0;
    int laneCount = 0;
    
    const char roads[] = {'A', 'B', 'C', 'D'};
    for (char road : roads) {
        for (int lane = 1; lane <= 3; lane++) {
            if (road == 'A' && lane == 2 && needsPriority()) continue;
            
            Queue& q = getQueue(road, lane);
            int s = size(q);
            if (s > 0) {
                totalVehicles += s;
                laneCount++;
            }
        }
    }
    
    if (laneCount == 0) return 1;
    int average = totalVehicles / laneCount;
    return max(1, average);
}

void switchTrafficLights() {
    lights[currentGreenLight].state = RED;
    currentGreenLight = (currentGreenLight + 1) % 4;
    lights[currentGreenLight].state = GREEN;
    
    cout << "\n════════════════════════════════════════\n";
    cout << "[TRAFFIC LIGHT] Road " << lights[currentGreenLight].road << " is now GREEN\n";
    cout << "════════════════════════════════════════\n";
}

SDL_Color getLaneColor(int lane) {
    if (lane == 1) return COLOR_LANE1;
    if (lane == 2) return COLOR_LANE2;
    return COLOR_LANE3;
}

void getVehicleStartPosition(char road, int lane, int queuePos, float& x, float& y) {
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    int spacing = 65;
    int laneOffset = (lane - 2) * LANE_WIDTH; // Centered on lane
    
    if (road == 'A') {
        x = centerX + laneOffset + 25;
        y = 60 + queuePos * spacing;
    } else if (road == 'B') {
        x = WINDOW_WIDTH - 60 - queuePos * spacing;
        y = centerY + laneOffset + 25;
    } else if (road == 'C') {
        x = centerX + laneOffset + 25;
        y = WINDOW_HEIGHT - 60 - queuePos * spacing;
    } else {
        x = 60 + queuePos * spacing;
        y = centerY + laneOffset + 25;
    }
}

TurnDirection getTurnDirection(int lane) {
    return (lane == 3) ? LEFT : STRAIGHT;
}

void generateWaypoints(AnimatedVehicle& av, int centerX, int centerY) {
    av.waypoints.clear();
    av.currentWaypoint = 0;
    
    int halfJunction = JUNCTION_SIZE / 2;
    int laneOffset = (av.lane - 2) * LANE_WIDTH;
    
    av.waypoints.push_back({av.x, av.y});
    
    if (av.direction == LEFT) {
        // LEFT TURN - Each road turns to previous road (A→D, B→A, C→B, D→C)
        if (av.road == 'A') {
            float exitLane = 25; // Lane 1 of destination road
            av.waypoints.push_back({centerX + laneOffset + 25, centerY - halfJunction - 30});
            av.waypoints.push_back({centerX - 50, centerY - 50});
            av.waypoints.push_back({centerX - halfJunction - 30, centerY + exitLane});
            av.waypoints.push_back({20, centerY + exitLane});
        } else if (av.road == 'B') {
            float exitLane = 25;
            av.waypoints.push_back({centerX + halfJunction + 30, centerY + laneOffset + 25});
            av.waypoints.push_back({centerX + 50, centerY - 50});
            av.waypoints.push_back({centerX + exitLane, centerY - halfJunction - 30});
            av.waypoints.push_back({centerX + exitLane, 20});
        } else if (av.road == 'C') {
            float exitLane = 25;
            av.waypoints.push_back({centerX + laneOffset + 25, centerY + halfJunction + 30});
            av.waypoints.push_back({centerX + 50, centerY + 50});
            av.waypoints.push_back({centerX + halfJunction + 30, centerY + exitLane});
            av.waypoints.push_back({WINDOW_WIDTH - 20, centerY + exitLane});
        } else {
            float exitLane = 25;
            av.waypoints.push_back({centerX - halfJunction - 30, centerY + laneOffset + 25});
            av.waypoints.push_back({centerX - 50, centerY + 50});
            av.waypoints.push_back({centerX + exitLane, centerY + halfJunction + 30});
            av.waypoints.push_back({centerX + exitLane, WINDOW_HEIGHT - 20});
        }
    } else {
        // STRAIGHT - Stay in lane
        if (av.road == 'A') {
            av.waypoints.push_back({centerX + laneOffset + 25, centerY - halfJunction});
            av.waypoints.push_back({centerX + laneOffset + 25, centerY + halfJunction});
            av.waypoints.push_back({centerX + laneOffset + 25, WINDOW_HEIGHT - 20});
        } else if (av.road == 'B') {
            av.waypoints.push_back({centerX + halfJunction, centerY + laneOffset + 25});
            av.waypoints.push_back({centerX - halfJunction, centerY + laneOffset + 25});
            av.waypoints.push_back({20, centerY + laneOffset + 25});
        } else if (av.road == 'C') {
            av.waypoints.push_back({centerX + laneOffset + 25, centerY + halfJunction});
            av.waypoints.push_back({centerX + laneOffset + 25, centerY - halfJunction});
            av.waypoints.push_back({centerX + laneOffset + 25, 20});
        } else {
            av.waypoints.push_back({centerX - halfJunction, centerY + laneOffset + 25});
            av.waypoints.push_back({centerX + halfJunction, centerY + laneOffset + 25});
            av.waypoints.push_back({WINDOW_WIDTH - 20, centerY + laneOffset + 25});
        }
    }
    
    if (av.waypoints.size() > 1) {
        av.currentWaypoint = 1;
    }
}

void updateMovingVehicles(float deltaTime) {
    for (auto it = movingVehicles.begin(); it != movingVehicles.end();) {
        // Handle delay timer
        if (it->delayTimer > 0) {
            it->delayTimer -= deltaTime;
            ++it;
            continue;
        }
        
        if (it->isMoving && it->currentWaypoint < (int)it->waypoints.size()) {
            float targetX = it->waypoints[it->currentWaypoint].first;
            float targetY = it->waypoints[it->currentWaypoint].second;
            
            float dx = targetX - it->x;
            float dy = targetY - it->y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance < 8.0f) {
                it->currentWaypoint++;
                
                if (it->currentWaypoint >= (int)it->waypoints.size()) {
                    it = movingVehicles.erase(it);
                    continue;
                }
            } else {
                float moveX = (dx / distance) * it->speed * deltaTime;
                float moveY = (dy / distance) * it->speed * deltaTime;
                it->x += moveX;
                it->y += moveY;
            }
        }
        ++it;
    }
}

void addVehiclesFromQueue(Queue& q, char road, int lane, int count = 1) {
    if (isEmpty(q)) return;
    
    bool roadIsGreen = false;
    for (int i = 0; i < 4; i++) {
        if (lights[i].road == road && lights[i].state == GREEN) {
            roadIsGreen = true;
            break;
        }
    }
    
    if (!roadIsGreen) {
        return;
    }
    
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    int vehiclesAdded = 0;
    
    while (!isEmpty(q) && vehiclesAdded < count) {
        Vehicle v = dequeue(q);
        
        AnimatedVehicle av;
        av.id = v.id;
        av.road = road;
        av.lane = lane;
        av.isMoving = true;
        av.color = getLaneColor(lane);
        av.speed = 100.0f + (rand() % 50);
        av.direction = getTurnDirection(lane);
        av.delayTimer = vehiclesAdded * 0.8f; // Stagger by 0.8 seconds
        
        getVehicleStartPosition(road, lane, 0, av.x, av.y);
        generateWaypoints(av, centerX, centerY);
        
        movingVehicles.push_back(av);
        
        string dirStr = (av.direction == LEFT) ? "LEFT" : "STRAIGHT";
        cout << "    ↳ Vehicle #" << v.id << " [" << road << "L" << lane 
             << "] " << dirStr << "\n";
        vehiclesAdded++;
    }
}

void serveCurrentGreenRoad() {
    char road = lights[currentGreenLight].road;
    
    if (needsPriority()) {
        cout << "\n🚨 PRIORITY MODE - AL2: " << size(AL2) << " vehicles\n";
        
        int servedCount = 0;
        while (size(AL2) >= 5 && !isEmpty(AL2)) {
            addVehiclesFromQueue(AL2, 'A', 2, 1);
            servedCount++;
        }
        cout << "✓ Served " << servedCount << " priority vehicles\n";
        return;
    }
    
    int avgVehicles = calculateVehiclesToServe();
    
    cout << "\n🚦 Road " << road << " GREEN - Serving " << avgVehicles << " vehicles per lane\n";
    
    for (int lane = 1; lane <= 3; lane++) {
        Queue& q = getQueue(road, lane);
        if (!isEmpty(q)) {
            addVehiclesFromQueue(q, road, lane, avgVehicles);
        }
    }
}

int getTotalVehiclesInQueues() {
    return size(AL1) + size(AL2) + size(AL3) +
           size(BL1) + size(BL2) + size(BL3) +
           size(CL1) + size(CL2) + size(CL3) +
           size(DL1) + size(DL2) + size(DL3);
}

void drawFilledRect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_FRect rect = {(float)x, (float)y, (float)w, (float)h};
    SDL_RenderFillRect(renderer, &rect);
}

void drawCircle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

void drawText(SDL_Renderer* renderer, const string& text, int x, int y, SDL_Color color, int size = 2) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int charWidth = 4 * size;
    int charHeight = 6 * size;
    
    for (size_t i = 0; i < text.length(); i++) {
        if (text[i] != ' ') {
            SDL_FRect rect = {(float)(x + i * charWidth), (float)y, (float)(charWidth - size), (float)charHeight};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void drawCar(SDL_Renderer* renderer, float x, float y, int id, SDL_Color color, bool isVertical) {
    int width = isVertical ? 40 : 50;
    int height = isVertical ? 50 : 40;
    
    drawFilledRect(renderer, x + 2, y + 2, width, height, {0, 0, 0, 80});
    drawFilledRect(renderer, x, y, width, height, color);
    
    SDL_Color highlight = {
        (Uint8)min(255, color.r + 50),
        (Uint8)min(255, color.g + 50),
        (Uint8)min(255, color.b + 50),
        255
    };
    if (isVertical) {
        drawFilledRect(renderer, x, y, width, 6, highlight);
    } else {
        drawFilledRect(renderer, x, y, 6, height, highlight);
    }
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_FRect carRect = {x, y, (float)width, (float)height};
    SDL_RenderRect(renderer, &carRect);
    
    string idStr = to_string(id);
    drawText(renderer, idStr, x + width/2 - 4, y + height/2 - 3, {255, 255, 255, 255}, 1);
}

void drawRoad(SDL_Renderer* renderer, int x, int y, int width, int height, bool isVertical) {
    drawFilledRect(renderer, x, y, width, height, COLOR_ROAD);
    
    SDL_SetRenderDrawColor(renderer, COLOR_LANE_LINE.r, COLOR_LANE_LINE.g, COLOR_LANE_LINE.b, 255);
    
    if (isVertical) {
        for (int lane = 1; lane < 3; lane++) {
            int lineX = x + lane * LANE_WIDTH;
            for (int dashY = y; dashY < y + height; dashY += 30) {
                SDL_FRect dash = {(float)lineX - 1.5f, (float)dashY, 3.0f, 18.0f};
                SDL_RenderFillRect(renderer, &dash);
            }
        }
    } else {
        for (int lane = 1; lane < 3; lane++) {
            int lineY = y + lane * LANE_WIDTH;
            for (int dashX = x; dashX < x + width; dashX += 30) {
                SDL_FRect dash = {(float)dashX, (float)lineY - 1.5f, 18.0f, 3.0f};
                SDL_RenderFillRect(renderer, &dash);
            }
        }
    }
}

void drawQueuedVehicles(SDL_Renderer* renderer, Queue& q, char road, int lane, bool isVertical) {
    int qSize = size(q);
    int displayCount = min(qSize, 8);
    
    for (int i = 0; i < displayCount; i++) {
        int index = (q.front + i) % MAX;
        if (q.front == -1) break;
        
        int vehicleID = q.data[index].id;
        SDL_Color carColor = getLaneColor(lane);
        
        float x, y;
        getVehicleStartPosition(road, lane, i, x, y);
        drawCar(renderer, x, y, vehicleID, carColor, isVertical);
    }
    
    if (qSize > 8) {
        float x, y;
        getVehicleStartPosition(road, lane, 8, x, y);
        drawText(renderer, "+" + to_string(qSize - 8), x, y, COLOR_YELLOW, 3);
    }
}

void drawTrafficLight(SDL_Renderer* renderer, int x, int y, LightState state, char road) {
    // Larger traffic light box
    drawFilledRect(renderer, x, y, 70, 150, {25, 25, 25, 255});
    drawFilledRect(renderer, x + 2, y + 2, 66, 146, {40, 40, 40, 255});
    
    // RED light (top)
    if (state == RED) {
        drawCircle(renderer, x + 35, y + 40, 25, {220, 38, 38, 255});
        drawCircle(renderer, x + 35, y + 40, 28, {180, 20, 20, 100});
    } else {
        drawCircle(renderer, x + 35, y + 40, 25, {60, 20, 20, 255});
    }
    
    // GREEN light (bottom)
    if (state == GREEN) {
        drawCircle(renderer, x + 35, y + 110, 25, {34, 197, 94, 255});
        drawCircle(renderer, x + 35, y + 110, 28, {20, 150, 70, 100});
    } else {
        drawCircle(renderer, x + 35, y + 110, 25, {20, 60, 30, 255});
    }
    
    // Road label
    string label = "Road ";
    label += road;
    drawText(renderer, label, x + 5, y - 25, COLOR_TEXT, 2);
}

void drawJunction(SDL_Renderer* renderer) {
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    int halfJunction = JUNCTION_SIZE / 2;
    
    drawFilledRect(renderer, centerX - halfJunction, centerY - halfJunction, 
                   JUNCTION_SIZE, JUNCTION_SIZE, COLOR_JUNCTION);
    
    int roadAX = centerX - halfJunction;
    int roadAY = 50;
    int roadAHeight = centerY - halfJunction - roadAY;
    drawRoad(renderer, roadAX, roadAY, ROAD_WIDTH, roadAHeight, true);
    drawTrafficLight(renderer, centerX - 100, centerY - halfJunction - 180, lights[0].state, 'A');
    
    int roadBX = centerX + halfJunction;
    int roadBY = centerY - halfJunction;
    int roadBWidth = WINDOW_WIDTH - roadBX - 50;
    drawRoad(renderer, roadBX, roadBY, roadBWidth, ROAD_WIDTH, false);
    drawTrafficLight(renderer, centerX + halfJunction + 30, centerY - 100, lights[1].state, 'B');
    
    int roadCX = centerX - halfJunction;
    int roadCY = centerY + halfJunction;
    int roadCHeight = WINDOW_HEIGHT - roadCY - 50;
    drawRoad(renderer, roadCX, roadCY, ROAD_WIDTH, roadCHeight, true);
    drawTrafficLight(renderer, centerX - 100, centerY + halfJunction + 30, lights[2].state, 'C');
    
    int roadDX = 50;
    int roadDY = centerY - halfJunction;
    int roadDWidth = centerX - halfJunction - roadDX;
    drawRoad(renderer, roadDX, roadDY, roadDWidth, ROAD_WIDTH, false);
    drawTrafficLight(renderer, centerX - halfJunction - 180, centerY - 100, lights[3].state, 'D');
    
    drawQueuedVehicles(renderer, AL1, 'A', 1, true);
    drawQueuedVehicles(renderer, AL2, 'A', 2, true);
    drawQueuedVehicles(renderer, AL3, 'A', 3, true);
    drawQueuedVehicles(renderer, BL1, 'B', 1, false);
    drawQueuedVehicles(renderer, BL2, 'B', 2, false);
    drawQueuedVehicles(renderer, BL3, 'B', 3, false);
    drawQueuedVehicles(renderer, CL1, 'C', 1, true);
    drawQueuedVehicles(renderer, CL2, 'C', 2, true);
    drawQueuedVehicles(renderer, CL3, 'C', 3, true);
    drawQueuedVehicles(renderer, DL1, 'D', 1, false);
    drawQueuedVehicles(renderer, DL2, 'D', 2, false);
    drawQueuedVehicles(renderer, DL3, 'D', 3, false);
    
    drawText(renderer, "A-L1:" + to_string(size(AL1)), roadAX + 5, 20, COLOR_LANE1, 2);
    drawText(renderer, "A-L2:" + to_string(size(AL2)), roadAX + 110, 20, COLOR_LANE2, 2);
    drawText(renderer, "A-L3:" + to_string(size(AL3)), roadAX + 215, 20, COLOR_LANE3, 2);
    
    drawText(renderer, "B-L1:" + to_string(size(BL1)), WINDOW_WIDTH - 160, roadBY + 5, COLOR_LANE1, 2);
    drawText(renderer, "B-L2:" + to_string(size(BL2)), WINDOW_WIDTH - 160, roadBY + 105, COLOR_LANE2, 2);
    drawText(renderer, "B-L3:" + to_string(size(BL3)), WINDOW_WIDTH - 160, roadBY + 205, COLOR_LANE3, 2);
    
    drawText(renderer, "C-L1:" + to_string(size(CL1)), roadCX + 5, WINDOW_HEIGHT - 30, COLOR_LANE1, 2);
    drawText(renderer, "C-L2:" + to_string(size(CL2)), roadCX + 110, WINDOW_HEIGHT - 30, COLOR_LANE2, 2);
    drawText(renderer, "C-L3:" + to_string(size(CL3)), roadCX + 215, WINDOW_HEIGHT - 30, COLOR_LANE3, 2);
    
    drawText(renderer, "D-L1:" + to_string(size(DL1)), 10, roadDY + 5, COLOR_LANE1, 2);
    drawText(renderer, "D-L2:" + to_string(size(DL2)), 10, roadDY + 105, COLOR_LANE2, 2);
    drawText(renderer, "D-L3:" + to_string(size(DL3)), 10, roadDY + 205, COLOR_LANE3, 2);
    
    for (const auto& v : movingVehicles) {
        if (v.delayTimer <= 0) {
            bool isVertical = (v.road == 'A' || v.road == 'C');
            drawCar(renderer, v.x, v.y, v.id, v.color, isVertical);
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        cout << "SDL Error: " << SDL_GetError() << endl;
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Traffic Junction Simulator", 
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    initAllQueues();
    srand(time(0));
    
    const char roads[] = {'A', 'B', 'C', 'D'};
    for (char road : roads) {
        for (int lane = 1; lane <= 3; lane++) {
            string filename = "lane_" + string(1, road) + to_string(lane) + ".txt";
            ofstream file(filename, ios::trunc);
            file.close();
        }
    }
    
    bool running = true;
    SDL_Event event;
    auto lastTime = chrono::steady_clock::now();
    auto lastGenTime = lastTime;
    auto lastSimTime = lastTime;
    
    cout << "\n╔═══════════════════════════════════════╗\n";
    cout << "║   TRAFFIC JUNCTION SIMULATOR         ║\n";
    cout << "╚═══════════════════════════════════════╝\n\n";
    cout << "✅ No collisions (staggered release)\n";
    cout << "✅ Better traffic lights (bigger & clearer)\n";
    cout << "✅ Fixed lane paths\n\n";
    cout << "Controls:\n";
    cout << "  [SPACE] Generate vehicles\n";
    cout << "  [ENTER] Process traffic\n";
    cout << "  [ESC]   Exit\n\n";
    
    while (running) {
        auto currentTime = chrono::steady_clock::now();
        float deltaTime = chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) running = false;
                else if (event.key.key == SDLK_SPACE) {
                    for (int i = 0; i < 5; i++) generateVehicle();
                    loadVehiclesFromFiles();
                }
                else if (event.key.key == SDLK_RETURN) {
                    loadVehiclesFromFiles();
                    serveCurrentGreenRoad();
                    switchTrafficLights();
                }
            }
        }
        
        if (chrono::duration_cast<chrono::milliseconds>(currentTime - lastGenTime).count() >= 2000) {
            int numToGenerate = 2 + rand() % 4;
            for (int i = 0; i < numToGenerate; i++) generateVehicle();
            loadVehiclesFromFiles();
            lastGenTime = currentTime;
        }
        
        if (chrono::duration_cast<chrono::seconds>(currentTime - lastSimTime).count() >= 6) {
            loadVehiclesFromFiles();
            serveCurrentGreenRoad();
            switchTrafficLights();
            lastSimTime = currentTime;
        }
        
        updateMovingVehicles(deltaTime);
        
        SDL_SetRenderDrawColor(renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, 255);
        SDL_RenderClear(renderer);
        
        drawFilledRect(renderer, 0, 0, WINDOW_WIDTH, 90, {20, 29, 47, 255});
        drawText(renderer, "TRAFFIC JUNCTION - ASSIGNMENT 1", 30, 20, COLOR_TEXT, 4);
        drawText(renderer, "Total Generated: " + to_string(vehicleId - 1), 30, 60, COLOR_GREEN, 2);
        drawText(renderer, "In Queue: " + to_string(getTotalVehiclesInQueues()), 320, 60, COLOR_YELLOW, 2);
        drawText(renderer, "Moving: " + to_string(movingVehicles.size()), 560, 60, COLOR_TEXT, 2);
        
        if (needsPriority()) {
            drawFilledRect(renderer, WINDOW_WIDTH - 480, 15, 460, 60, {127, 29, 29, 255});
            drawText(renderer, "PRIORITY MODE ACTIVE!", WINDOW_WIDTH - 460, 25, {255, 255, 255, 255}, 3);
            drawText(renderer, "AL2: " + to_string(size(AL2)) + " vehicles (>10)", WINDOW_WIDTH - 460, 50, COLOR_YELLOW, 2);
        }
        
        drawJunction(renderer);
        
        drawFilledRect(renderer, 0, WINDOW_HEIGHT - 70, WINDOW_WIDTH, 70, {20, 29, 47, 255});
        
        string greenRoad = "Current GREEN: Road ";
        greenRoad += lights[currentGreenLight].road;
        drawText(renderer, greenRoad, 30, WINDOW_HEIGHT - 50, COLOR_GREEN, 3);
        
        string redRoads = "RED: ";
        for (int i = 0; i < 4; i++) {
            if (i != currentGreenLight) {
                redRoads += lights[i].road;
                redRoads += " ";
            }
        }
        drawText(renderer, redRoads, 400, WINDOW_HEIGHT - 50, COLOR_RED, 3);
        
        drawText(renderer, "L1=Incoming | L2=Traffic | L3=Left Turn", 
                WINDOW_WIDTH - 640, WINDOW_HEIGHT - 45, COLOR_TEXT, 2);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}