/**
 * @file pendulum.cpp
 * @brief a simple animation of a non-uniform circular motion
 * @date 6th, oct 2024
 * 
 */

#include <iostream>
#include <chrono>
#include <cmath>
#include <vector>

#include <SDL.h>
#include <phy/Vector.h>

using namespace phy;

struct Canvas {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event evt;
    int w;
    int h;
};


// rigid body types
struct
{
    Vector2 pos;
    Vector2 vel;
    float mass = 1.0f;
    float im = 5000;
    float angVel = 0.0f;
    float angDispl = 0.0f;
    std::vector<Vector2> vertices;

} rigidBody;

Vector2 floorStart, floorEnd;


void init(const Canvas& cnv) {
    rigidBody.vertices.push_back({-50, -25});
    rigidBody.vertices.push_back({50, -25});
    rigidBody.vertices.push_back({50, 25});
    rigidBody.vertices.push_back({-50, 25});

    rigidBody.pos.x = cnv.w / 2;
    rigidBody.pos.y = cnv.h / 2;

    rigidBody.angVel = 2 * 3.14159f * 0.03f;
}


void update(float dt, Canvas& canvas) 
{
    rigidBody.angDispl = rigidBody.angVel * dt;

    for(auto it = rigidBody.vertices.begin(); it != rigidBody.vertices.end(); it++) {
        *it = it->rotate(rigidBody.angDispl);
    }
}


void render(Canvas& canvas) 
{
    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0x00, 0x34, 0xff);
    for(size_t i = 0; i < rigidBody.vertices.size(); i++)
    {
        auto cPos = rigidBody.pos + rigidBody.vertices[i];
        auto nPos = rigidBody.pos + rigidBody.vertices[(i + 1) % rigidBody.vertices.size()];
        SDL_RenderDrawLineF(canvas.renderer, cPos.x, cPos.y, nPos.x, nPos.y);

        if(i == 0)
            SDL_RenderDrawLineF(canvas.renderer, rigidBody.pos.x, rigidBody.pos.y, cPos.x, cPos.y);
    }

}



void mainLoop(Canvas& canvas) {
    bool shouldClose = false;

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!shouldClose)
    {
        while(SDL_PollEvent(&canvas.evt) != 0)
        {
            if(canvas.evt.type == SDL_QUIT) 
            shouldClose = true;
        }

        SDL_SetRenderDrawColor(canvas.renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderClear(canvas.renderer);
        render(canvas);

        auto now = std::chrono::high_resolution_clock::now();
        auto dt = (now - lastTime).count() * 10e-9;
        lastTime = now;

        update(dt, canvas);

        SDL_RenderPresent(canvas.renderer);
    }
    
}


bool initCanvas(Canvas& canvas, int w, int h) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL failed to initialize" << std::endl;
        return false;
    }

    canvas.window = SDL_CreateWindow("RigidBody", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    if(!canvas.window) {
        std::cerr << "SDL failed to create window" << std::endl;
        return false;
    }
    canvas.w = w;
    canvas.h = h;

    canvas.renderer = SDL_CreateRenderer(canvas.window, 0, SDL_RENDERER_ACCELERATED);
    if(!canvas.renderer) {
        std::cerr << "SDL failed to create renderer" << std::endl;
        return false;
    }

    return true;
}


int main() {
    Canvas cnv;
    if(!initCanvas(cnv, 640, 480)) return -1;
    init(cnv);
    mainLoop(cnv);
    return 0;
}