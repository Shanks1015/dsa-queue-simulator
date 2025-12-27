#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include "queue.h"

using namespace std;

// External functions
extern void initAllQueues();
extern void loadVehiclesFromFiles();
extern Queue& getQueue(char road, int lane);
extern void generateVehicle();
extern int vehicleId;

extern Queue AL1, AL2, AL3, BL1, BL2, BL3, CL1, CL2, CL3, DL1, DL2, DL3;

const int WINDOW_WIDTH = 1800;
const int WINDOW_HEIGHT = 1000;
const int ROAD_WIDTH = 300;
const int LANE_WIDTH = 100;
const int JUNCTION_SIZE = 300;

// Colors
SDL_Color COLOR_BG = {15, 23, 42, 255};
SDL_Color COLOR_ROAD = {55, 65, 81, 255};
SDL_Color COLOR_JUNCTION = {30, 41, 59, 255};
SDL_Color COLOR_LANE_LINE = {251, 191, 36, 255};
SDL_Color COLOR_LANE1 = {59, 130, 246, 255};
SDL_Color COLOR_LANE2 = {251, 146, 60, 255};
SDL_Color COLOR_LANE3 = {34, 197, 94, 255};
SDL_Color COLOR_CAR_BLUE = {96, 165, 250, 255};
SDL_Color COLOR_CAR_ORANGE = {253, 186, 116, 255};
SDL_Color COLOR_CAR_GREEN = {134, 239, 172, 255};
SDL_Color COLOR_TEXT = {241, 245, 249, 255};
SDL_Color COLOR_GREEN = {34, 197, 94, 255};
SDL_Color COLOR_RED = {239, 68, 68, 255};
SDL_Color COLOR_YELLOW = {251, 191, 36, 255};

// Animated vehicle
struct AnimatedVehicle {
    int id;
    char road;
    int lane;
    float x, y;
    float targetX, targetY;
    bool isMoving;
    SDL_Color color;
    float speed;
};

vector<AnimatedVehicle> movingVehicles;

enum LightState { RED = 0, GREEN = 1 };

struct TrafficLight {
    LightState state;
    char road;
};

// CORRECTED: Only ONE road green at a time (as per PDF)
TrafficLight lights[4] = {{GREEN, 'A'}, {RED, 'B'}, {RED, 'C'}, {RED, 'D'}};
int currentGreenLight = 0; // 0=A, 1=B, 2=C, 3=D

// CORRECTED: Priority threshold is > 5 (not > 10)
bool needsPriority() {
    return size(AL2) > 5;  // PDF says: "more than 5 vehicles"
}

// CORRECTED: Calculate average using PDF formula
// |V| = (1/n) × Σ|Li|
int calculateVehiclesToServe() {
    int totalVehicles = 0;
    int laneCount = 0;
    
    // Count vehicles in all normal lanes (excluding AL2 priority)
    const char roads[] = {'A', 'B', 'C', 'D'};
    for (char road : roads) {
        for (int lane = 1; lane <= 3; lane++) {
            if (road == 'A' && lane == 2) continue; // Skip AL2 (priority lane)
            
            Queue& q = getQueue(road, lane);
            int s = size(q);
            if (s > 0) {
                totalVehicles += s;
                laneCount++;
            }
        }
    }
    
    if (laneCount == 0) return 1;
    
    // Apply PDF formula: average = total / count
    int average = totalVehicles / laneCount;
    return max(1, average);
}

// CORRECTED: Only ONE road gets green light
void switchTrafficLights() {
    // Turn current light to RED
    lights[currentGreenLight].state = RED;
    
    // Move to next road
    currentGreenLight = (currentGreenLight + 1) % 4;
    
    // Turn only the new road to GREEN
    lights[currentGreenLight].state = GREEN;
    
    cout << "\n════════════════════════════════════════\n";
    cout << "[TRAFFIC LIGHT] Road " << lights[currentGreenLight].road 
         << " is now GREEN\n";
    cout << "[TRAFFIC LIGHT] Roads ";
    for (int i = 0; i < 4; i++) {
        if (i != currentGreenLight) cout << lights[i].road << " ";
    }
    cout << "are RED\n";
    cout << "════════════════════════════════════════\n";
}

SDL_Color getLaneColor(int lane) {
    if (lane == 1) return COLOR_CAR_BLUE;
    if (lane == 2) return COLOR_CAR_ORANGE;
    return COLOR_CAR_GREEN;
}

void getVehicleStartPosition(char road, int lane, int queuePos, float& x, float& y) {
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    int spacing = 65;
    int halfJunction = JUNCTION_SIZE / 2;
    int laneOffset = (lane - 1) * LANE_WIDTH - LANE_WIDTH;
    
    if (road == 'A') {
        x = centerX + laneOffset + (LANE_WIDTH - 50) / 2;
        y = 60 + queuePos * spacing;
    } else if (road == 'B') {
        x = WINDOW_WIDTH - 60 - queuePos * spacing;
        y = centerY + laneOffset + (LANE_WIDTH - 50) / 2;
    } else if (road == 'C') {
        x = centerX - laneOffset + (LANE_WIDTH - 50) / 2;
        y = WINDOW_HEIGHT - 60 - queuePos * spacing;
    } else {
        x = 60 + queuePos * spacing;
        y = centerY - laneOffset + (LANE_WIDTH - 50) / 2;
    }
}

void updateMovingVehicles(float deltaTime) {
    for (auto it = movingVehicles.begin(); it != movingVehicles.end();) {
        if (it->isMoving) {
            float dx = it->targetX - it->x;
            float dy = it->targetY - it->y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance < 5.0f) {
                it = movingVehicles.erase(it);
                continue;
            }
            
            float moveX = (dx / distance) * it->speed * deltaTime;
            float moveY = (dy / distance) * it->speed * deltaTime;
            it->x += moveX;
            it->y += moveY;
        }
        ++it;
    }
}

void addVehiclesFromQueue(Queue& q, char road, int lane, int count = 1) {
    if (isEmpty(q)) return;
    
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
        av.speed = 120.0f + (rand() % 60);
        
        getVehicleStartPosition(road, lane, 0, av.x, av.y);
        
        int laneOffset = (lane - 1) * LANE_WIDTH - LANE_WIDTH;
        
        if (road == 'A') {
            av.targetX = centerX + laneOffset + (LANE_WIDTH - 50) / 2;
            av.targetY = WINDOW_HEIGHT - 40;
        } else if (road == 'B') {
            av.targetX = 40;
            av.targetY = centerY + laneOffset + (LANE_WIDTH - 50) / 2;
        } else if (road == 'C') {
            av.targetX = centerX - laneOffset + (LANE_WIDTH - 50) / 2;
            av.targetY = 40;
        } else {
            av.targetX = WINDOW_WIDTH - 40;
            av.targetY = centerY - laneOffset + (LANE_WIDTH - 50) / 2;
        }
        
        movingVehicles.push_back(av);
        cout << "    ↳ Vehicle #" << v.id << " [" << road << "L" << lane << "] passing\n";
        vehiclesAdded++;
    }
}

// CORRECTED: Serve vehicles according to PDF algorithm
void serveCurrentGreenRoad() {
    char road = lights[currentGreenLight].road;
    
    // STEP 1: Check priority condition FIRST (as per PDF)
    if (needsPriority()) {
        cout << "\n┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n";
        cout << "┃  🚨 PRIORITY MODE ACTIVATED! 🚨     ┃\n";
        cout << "┃  AL2 has " << size(AL2) << " vehicles (threshold: >5)  ┃\n";
        cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n";
        
        // Serve AL2 until it drops below 5
        int servedCount = 0;
        while (size(AL2) > 5 && !isEmpty(AL2)) {
            addVehiclesFromQueue(AL2, 'A', 2, 1);
            servedCount++;
        }
        cout << "  ✓ Served " << servedCount << " priority vehicles from AL2\n";
        cout << "  ✓ AL2 now has " << size(AL2) << " vehicles (below threshold)\n";
        return;
    }
    
    // STEP 2: Normal condition - use average formula
    int avgVehicles = calculateVehiclesToServe();
    
    cout << "\n┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n";
    cout << "┃  🚦 NORMAL CONDITION - Road " << road << "        ┃\n";
    cout << "┃  Average formula: |V| = " << avgVehicles << "          ┃\n";
    cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n";
    
    // Serve avgVehicles from each lane of current green road
    for (int lane = 1; lane <= 3; lane++) {
        Queue& q = getQueue(road, lane);
        if (!isEmpty(q)) {
            cout << "  [" << road << "L" << lane << "] Serving " << avgVehicles 
                 << " vehicles (queue size: " << size(q) << ")\n";
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

void drawRect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color, int thickness = 2) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int i = 0; i < thickness; i++) {
        SDL_FRect rect = {(float)(x + i), (float)(y + i), (float)(w - 2*i), (float)(h - 2*i)};
        SDL_RenderRect(renderer, &rect);
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
    int width = isVertical ? 45 : 55;
    int height = isVertical ? 55 : 45;
    
    drawFilledRect(renderer, x + 3, y + 3, width, height, {0, 0, 0, 100});
    drawFilledRect(renderer, x, y, width, height, color);
    
    SDL_Color highlight = {
        (Uint8)min(255, color.r + 40),
        (Uint8)min(255, color.g + 40),
        (Uint8)min(255, color.b + 40),
        255
    };
    if (isVertical) {
        drawFilledRect(renderer, x, y, width, 8, highlight);
    } else {
        drawFilledRect(renderer, x, y, 8, height, highlight);
    }
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_FRect carRect = {x, y, (float)width, (float)height};
    SDL_RenderRect(renderer, &carRect);
    
    SDL_Color windowColor = {
        (Uint8)(color.r * 0.4),
        (Uint8)(color.g * 0.4),
        (Uint8)(color.b * 0.4),
        255
    };
    
    if (isVertical) {
        drawFilledRect(renderer, x + 4, y + 4, width - 8, 14, windowColor);
        drawFilledRect(renderer, x + 4, y + height - 18, width - 8, 14, windowColor);
    } else {
        drawFilledRect(renderer, x + 4, y + 4, 14, height - 8, windowColor);
        drawFilledRect(renderer, x + width - 18, y + 4, 14, height - 8, windowColor);
    }
    
    string idStr = to_string(id);
    drawText(renderer, idStr, x + width/2 - 6, y + height/2 - 4, {255, 255, 255, 255}, 2);
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
    int displayCount = min(qSize, 10);
    
    for (int i = 0; i < displayCount; i++) {
        int index = (q.front + i) % MAX;
        if (q.front == -1) break;
        
        int vehicleID = q.data[index].id;
        SDL_Color carColor = getLaneColor(lane);
        
        float x, y;
        getVehicleStartPosition(road, lane, i, x, y);
        drawCar(renderer, x, y, vehicleID, carColor, isVertical);
    }
    
    if (qSize > 10) {
        float x, y;
        getVehicleStartPosition(road, lane, 10, x, y);
        drawText(renderer, "+" + to_string(qSize - 10), x, y, COLOR_YELLOW, 2);
    }
}

void drawTrafficLight(SDL_Renderer* renderer, int x, int y, LightState state) {
    drawFilledRect(renderer, x, y, 40, 95, {20, 20, 20, 255});
    drawRect(renderer, x, y, 40, 95, {60, 60, 60, 255}, 2);
    
    if (state == RED) {
        drawFilledRect(renderer, x + 8, y + 8, 24, 24, {239, 68, 68, 255});
        for (int i = 1; i <= 4; i++) {
            SDL_SetRenderDrawColor(renderer, 239, 68, 68, 50 - i * 10);
            SDL_FRect glow = {(float)(x + 8 - i), (float)(y + 8 - i), (float)(24 + 2*i), (float)(24 + 2*i)};
            SDL_RenderRect(renderer, &glow);
        }
    } else {
        drawFilledRect(renderer, x + 8, y + 8, 24, 24, {60, 20, 20, 255});
    }
    
    if (state == GREEN) {
        drawFilledRect(renderer, x + 8, y + 63, 24, 24, {34, 197, 94, 255});
        for (int i = 1; i <= 4; i++) {
            SDL_SetRenderDrawColor(renderer, 34, 197, 94, 50 - i * 10);
            SDL_FRect glow = {(float)(x + 8 - i), (float)(y + 63 - i), (float)(24 + 2*i), (float)(24 + 2*i)};
            SDL_RenderRect(renderer, &glow);
        }
    } else {
        drawFilledRect(renderer, x + 8, y + 63, 24, 24, {20, 60, 30, 255});
    }
}

void drawJunction(SDL_Renderer* renderer) {
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    int halfJunction = JUNCTION_SIZE / 2;
    
    drawFilledRect(renderer, centerX - halfJunction, centerY - halfJunction, 
                   JUNCTION_SIZE, JUNCTION_SIZE, COLOR_JUNCTION);
    
    // Roads
    int roadAX = centerX - halfJunction;
    int roadAY = 50;
    int roadAHeight = centerY - halfJunction - roadAY;
    drawRoad(renderer, roadAX, roadAY, ROAD_WIDTH, roadAHeight, true);
    drawTrafficLight(renderer, centerX - 60, centerY - halfJunction - 110, lights[0].state);
    
    int roadBX = centerX + halfJunction;
    int roadBY = centerY - halfJunction;
    int roadBWidth = WINDOW_WIDTH - roadBX - 50;
    drawRoad(renderer, roadBX, roadBY, roadBWidth, ROAD_WIDTH, false);
    drawTrafficLight(renderer, centerX + halfJunction + 15, centerY - 60, lights[1].state);
    
    int roadCX = centerX - halfJunction;
    int roadCY = centerY + halfJunction;
    int roadCHeight = WINDOW_HEIGHT - roadCY - 50;
    drawRoad(renderer, roadCX, roadCY, ROAD_WIDTH, roadCHeight, true);
    drawTrafficLight(renderer, centerX - 60, centerY + halfJunction + 15, lights[2].state);
    
    int roadDX = 50;
    int roadDY = centerY - halfJunction;
    int roadDWidth = centerX - halfJunction - roadDX;
    drawRoad(renderer, roadDX, roadDY, roadDWidth, ROAD_WIDTH, false);
    drawTrafficLight(renderer, centerX - halfJunction - 110, centerY - 60, lights[3].state);
    
    // Queued vehicles
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
    
    // Labels
    drawText(renderer, "AL1:" + to_string(size(AL1)), roadAX + 5, 20, COLOR_LANE1, 2);
    drawText(renderer, "AL2:" + to_string(size(AL2)), roadAX + 105, 20, COLOR_LANE2, 2);
    drawText(renderer, "AL3:" + to_string(size(AL3)), roadAX + 205, 20, COLOR_LANE3, 2);
    
    drawText(renderer, "BL1:" + to_string(size(BL1)), WINDOW_WIDTH - 140, roadBY + 5, COLOR_LANE1, 2);
    drawText(renderer, "BL2:" + to_string(size(BL2)), WINDOW_WIDTH - 140, roadBY + 105, COLOR_LANE2, 2);
    drawText(renderer, "BL3:" + to_string(size(BL3)), WINDOW_WIDTH - 140, roadBY + 205, COLOR_LANE3, 2);
    
    drawText(renderer, "CL1:" + to_string(size(CL1)), roadCX + 5, WINDOW_HEIGHT - 30, COLOR_LANE1, 2);
    drawText(renderer, "CL2:" + to_string(size(CL2)), roadCX + 105, WINDOW_HEIGHT - 30, COLOR_LANE2, 2);
    drawText(renderer, "CL3:" + to_string(size(CL3)), roadCX + 205, WINDOW_HEIGHT - 30, COLOR_LANE3, 2);
    
    drawText(renderer, "DL1:" + to_string(size(DL1)), 10, roadDY + 5, COLOR_LANE1, 2);
    drawText(renderer, "DL2:" + to_string(size(DL2)), 10, roadDY + 105, COLOR_LANE2, 2);
    drawText(renderer, "DL3:" + to_string(size(DL3)), 10, roadDY + 205, COLOR_LANE3, 2);
    
    // Moving vehicles
    for (const auto& v : movingVehicles) {
        bool isVertical = (v.road == 'A' || v.road == 'C');
        drawCar(renderer, v.x, v.y, v.id, v.color, isVertical);
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        cout << "SDL Error: " << SDL_GetError() << endl;
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Traffic Junction - PDF Compliant Algorithm", 
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
    
    bool running = true;
    SDL_Event event;
    auto lastTime = chrono::steady_clock::now();
    auto lastGenTime = lastTime;
    auto lastSimTime = lastTime;
    
    cout << "\n╔═══════════════════════════════════════════════╗\n";
    cout << "║   TRAFFIC JUNCTION - PDF COMPLIANT ALGORITHM  ║\n";
    cout << "╚═══════════════════════════════════════════════╝\n\n";
    cout << "✓ Priority threshold: AL2 > 5 vehicles\n";
    cout << "✓ Average formula: |V| = (1/n) × Σ|Li|\n";
    cout << "✓ Only ONE road green at a time\n\n";
    cout << "Controls:\n";
    cout << "  [SPACE] Generate 3 vehicles\n";
    cout << "  [ENTER] Process & switch light\n";
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
                    for (int i = 0; i < 3; i++) generateVehicle();
                    loadVehiclesFromFiles();
                }
                else if (event.key.key == SDLK_RETURN) {
                    loadVehiclesFromFiles();
                    serveCurrentGreenRoad();
                    switchTrafficLights();
                }
            }
        }
        
        // Auto-generate
        if (chrono::duration_cast<chrono::milliseconds>(currentTime - lastGenTime).count() >= 2000) {
            int numToGenerate = 2 + rand() % 3;
            for (int i = 0; i < numToGenerate; i++) generateVehicle();
            loadVehiclesFromFiles();
            lastGenTime = currentTime;
        }
        
        // Auto-simulate
        if (chrono::duration_cast<chrono::seconds>(currentTime - lastSimTime).count() >= 5) {
            loadVehiclesFromFiles();
            serveCurrentGreenRoad();
            switchTrafficLights();
            lastSimTime = currentTime;
        }
        
        updateMovingVehicles(deltaTime);
        
        // Render
        SDL_SetRenderDrawColor(renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, 255);
        SDL_RenderClear(renderer);
        
        // Header
        drawFilledRect(renderer, 0, 0, WINDOW_WIDTH, 80, {20, 29, 47, 255});
        drawText(renderer, "TRAFFIC JUNCTION - ASSIGNMENT 1", 30, 15, COLOR_TEXT, 3);
        drawText(renderer, "Generated: " + to_string(vehicleId - 1), 30, 48, COLOR_GREEN, 2);
        drawText(renderer, "Queued: " + to_string(getTotalVehiclesInQueues()), 280, 48, COLOR_YELLOW, 2);
        drawText(renderer, "Avg Formula: " + to_string(calculateVehiclesToServe()), 500, 48, COLOR_TEXT, 2);
        
        if (needsPriority()) {
            drawText(renderer, "PRIORITY AL2:" + to_string(size(AL2)) + " >5!", 
                    WINDOW_WIDTH - 400, 28, COLOR_RED, 3);
        }
        
        drawJunction(renderer);
        
        // Footer
        drawFilledRect(renderer, 0, WINDOW_HEIGHT - 60, WINDOW_WIDTH, 60, {20, 29, 47, 255});
        drawText(renderer, "GREEN: Road " + string(1, lights[currentGreenLight].road), 
                30, WINDOW_HEIGHT - 40, COLOR_GREEN, 3);
        
        drawText(renderer, "L1=Incoming | L2=Traffic | L3=Free/Left", 
                WINDOW_WIDTH - 600, WINDOW_HEIGHT - 40, COLOR_TEXT, 2);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}