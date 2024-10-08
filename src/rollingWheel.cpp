/**
 * @file rollingWheel.cpp
 * @brief a simple implementation of rotational motion
 * @date 30th Sept, 2024
 * 
 * This file shows the implementation of rotational motion using a wheel as an example
 */

#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include <cmath>
#include <random>

#include <SDL.h>
#include <phy/Vector.h>

using namespace phy;


Vector2 pos, vel;
std::vector<Vector2> vertices;
float floorPos;
float angDisp;
float radius = 50;
float angVel = 2 * 3.14159 * (1 * 3.14159 / 180); //2_(pi)_frequency

struct Canvas {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event evt;
    int w;
    int h;
};


void init(const Canvas& cnv) {
    floorPos = cnv.h * 0.8;
    pos.x = radius * 1.2f;
    pos.y = floorPos - radius;
    vel.x = 40;
    vel.y = 0;

    for(size_t i = 0; i < 360; i++)
    {
        float a = i * 3.14159f / 180;
        float px = std::cos(a) * radius;
        float py = std::sin(a) * radius;
        vertices.push_back({ px, py });
    }
}


void update(float dt, Canvas& canvas) 
{
    pos += vel * dt;
    angDisp += angVel * dt;
    vel.x = radius * angVel;
}


void render(Canvas& canvas) 
{
    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0x00, 0x00, 0xff);
    for(auto& vertex: vertices)
    {
        auto&& transformed = pos + vertex;
        SDL_RenderDrawPoint(canvas.renderer, int(transformed.x), int(transformed.y));
    }

    for(size_t i = 0; i < 4; i++)
    {
        const float a = i * 90 * 3.14159f / 180;
        Vector2 p = pos + Vector2{ std::cos(a), std::sin(a) }.rotate(angDisp) * radius;
        SDL_RenderDrawLine(canvas.renderer, pos.x, pos.y, p.x, p.y);
    }

    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLine(canvas.renderer, 0, floorPos, canvas.w, floorPos);
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

    canvas.window = SDL_CreateWindow("Rolling Wheel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
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