#include <iostream>

#include <SDL.h>


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

    SDL_Event evt;
    bool shouldClose = false;
    while (!shouldClose)
    {
        SDL_PollEvent(&evt);
        if(evt.type == SDL_QUIT) shouldClose = true;
    }
    

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}