#include <SDL3/SDL.h>
#include <fstream>
#include <vector>
#include <iostream>

struct Vehicle {
    int id;
    char road;
    int lane;
};

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Traffic Visualization",
        800, 600,
        SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        std::vector<Vehicle> vehicles;
        ifstream file("laneA.txt");
        Vehicle v;
        while (file >> v.id >> v.road >> v.lane)
            vehicles.push_back(v);
        file.close();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (auto &veh : vehicles) {
            SDL_FRect rect = {50.0f, veh.lane * 120.0f, 60.0f, 40.0f};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    SDL_Quit();
}
