/**
 * @file Satellite1.cpp
 * @brief a simple animation of a satellite around a rotating earth
 * @date 1st, oct 2024
 * 
 * This is not a simulation but an animation of a satellite in a fixed position
 * about a rotating earth
 */

#include <iostream>
#include <chrono>
#include <cmath>

#include <SDL.h>
#include <phy/Vector.h>

using namespace phy;

Vector2 earthPos, satPos;
float earthRadius, satRadius;
float angDisp;
float angVel = 2 * 3.14159f * 0.004f;

void drawStrokedCircle(SDL_Renderer *renderer, Vector2& pos, float radius);

struct Canvas {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event evt;
    int w;
    int h;
};


void init(const Canvas& cnv) {
    earthPos.x = cnv.w * 0.5;
    earthPos.y = cnv.h * 0.5f;
    earthRadius = 50;

    satRadius = 10;
    satPos.x = earthPos.x;
}


void update(float dt, Canvas& canvas) 
{
    angDisp += angVel * dt;
    float r = earthRadius * 1.4f + satRadius;
    satPos = earthPos + Vector2{ 0, -1}.rotate(angDisp) * r;
}


void render(Canvas& canvas) 
{
    SDL_SetRenderDrawColor(canvas.renderer, 0x00, 0x00, 0xff, 0xff);
    drawStrokedCircle(canvas.renderer, earthPos, earthRadius);
    auto e = earthPos + Vector2{ 0, -1 }.rotate(angDisp) * earthRadius;
    SDL_RenderDrawLine(canvas.renderer, earthPos.x, earthPos.y, e.x, e.y);

    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
    drawStrokedCircle(canvas.renderer, satPos, satRadius);
    e = satPos + Vector2{ 0, 1}.rotate(angDisp) * satRadius;
    SDL_RenderDrawLine(canvas.renderer, satPos.x, satPos.y, e.x, e.y);
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

    canvas.window = SDL_CreateWindow("Satellite", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
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


void drawStrokedCircle(SDL_Renderer *renderer, Vector2& pos, float radius)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        // Draw the 8 symmetrical points of the circle
        SDL_RenderDrawPoint(renderer, pos.x + x, pos.y + y);
        SDL_RenderDrawPoint(renderer, pos.x - x, pos.y + y);
        SDL_RenderDrawPoint(renderer, pos.x + x, pos.y - y);
        SDL_RenderDrawPoint(renderer, pos.x - x, pos.y - y);
        SDL_RenderDrawPoint(renderer, pos.x + y, pos.y + x);
        SDL_RenderDrawPoint(renderer, pos.x - y, pos.y + x);
        SDL_RenderDrawPoint(renderer, pos.x + y, pos.y - x);
        SDL_RenderDrawPoint(renderer, pos.x - y, pos.y - x);

        // Update the decision parameter and points
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
