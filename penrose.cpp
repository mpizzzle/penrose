#include <SDL2/SDL.h>

void draw(SDL_Renderer* Renderer) {
    SDL_SetRenderDrawColor(Renderer, 0x33, 0x66, 0x99, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(Renderer);

    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    SDL_RenderDrawLine(Renderer, 0, 480, 320, 0);
    SDL_RenderDrawLine(Renderer, 640, 480, 320, 0);
    SDL_RenderDrawLine(Renderer, 0, 480, 640, 480);

    SDL_RenderPresent(Renderer);
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* Window = SDL_CreateWindow("penrose", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, 0);
    draw(Renderer);
    SDL_Delay(1000);
    SDL_Quit();

    return 0;
}
