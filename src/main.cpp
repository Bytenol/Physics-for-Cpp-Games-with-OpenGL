#include <iostream>

#include <SDL.h>
#include "../include/phy/Ball.h"



int main() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL unable to initialize" << std::endl;
        return -1;
    }

    auto window = SDL_CreateWindow("main", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
    640, 480, SDL_WINDOW_SHOWN);
    if(!window) {
        std::cerr << "SDL unable to create window" << std::endl;
        return -1;
    }

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        std::cerr << "SDL renderer failed to be created" << std::endl;
        return -1;
    }

    phy::Ball ball;
    ball.pos.x = 200;
    ball.pos.y = 200;

    SDL_Event evt;
    bool shouldClose = false;
    while (!shouldClose)
    {
        SDL_PollEvent(&evt);
        if(evt.type == SDL_QUIT) shouldClose = true;
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(renderer);
        
        ball.render(renderer);

        SDL_RenderPresent(renderer);
    }
    

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}