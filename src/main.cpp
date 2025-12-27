#include <SDL3/SDL.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include "queue.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

void drawQueue(SDL_Renderer* renderer, Queue &q) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green vehicles

    for (int i = q.front; i <= q.rear; ++i) {
        Vehicle &v = q.data[i];
        int x = 0, y = 0;

        switch (v.road) {
            case 'A': x = 50; break;
            case 'B': x = 200; break;
            case 'C': x = 350; break;
            case 'D': x = 500; break;
        }
        y = v.lane * 50;

       SDL_FRect rect = { float(x), float(y), 40.0f, 40.0f };
        SDL_RenderFillRectF(renderer, &rect);

    }
}

int main() {
    srand(time(0));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Traffic Simulator", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Queue q;
    initQueue(q);
    int vehicleId = 1;

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // Generate vehicle
        Vehicle v;
        v.id = vehicleId++;
        v.road = 'A' + rand() % 4;
        v.lane = 1 + rand() % 3;
        enqueue(q, v);

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw queue
        drawQueue(renderer, q);

        SDL_RenderPresent(renderer);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
