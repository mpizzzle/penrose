#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

class triangle {
public:
    SDL_Point* points;
    std::vector<triangle*> subtriangles;
};

class t123 : public triangle {
};

class t124 : public triangle {
};

void draw(SDL_Renderer* Renderer, std::vector<triangle> triangles) {
    SDL_SetRenderDrawColor(Renderer, 0x33, 0x66, 0x99, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(Renderer);

    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    for (triangle t : triangles) {
        SDL_RenderDrawLines(Renderer, t.points, 4);
    }

    SDL_RenderPresent(Renderer);
}

int main() {
    bool quit = false;

    SDL_DisplayMode current;
    SDL_Event event;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", 0, current.w, current.h, current.refresh_rate);

    std::vector<triangle> triangles;

    t123 t;
    SDL_Point points[4] = { {0, 480}, {320, 0}, {640, 480}, {0, 480} };
    t.points = points;
    triangles.push_back(t);

    SDL_Window* Window = SDL_CreateWindow("penrose", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, current.w, current.h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, 0);

    while (!quit) {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:
            quit = true;
            break;
        }

        draw(Renderer, triangles);
    }

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}
