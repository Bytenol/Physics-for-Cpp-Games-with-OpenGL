/**
 * @file bungeeJump.cpp
 * @brief a simple 
 * @date 09th Oct, 2024
 * 
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

void drawFilledCircle(SDL_Renderer *r, const Vector2& pos, float radius);


struct Canvas {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event evt;
    int w;
    int h;
};

Vector2 eqPoint, bjPoint;
Vector2 vel;
float mass = 5.0f;
float g = 10;
float kSpring = 0.9f;


void init(const Canvas& cnv) 
{
    eqPoint.x = cnv.w * 0.5f;
    eqPoint.y = 50;
    bjPoint.x = eqPoint.x;
    bjPoint.y = eqPoint.y;
    // vel.x = 30;
    vel.y = 20;
}


void update(float dt, Canvas& canvas) 
{
    bjPoint += vel * dt;
    Vector2 weight{ 0, mass * g };

    float cordLength = 100.0f;
    auto dist = (eqPoint - bjPoint);
    Vector2 spring = dist * kSpring;
    if(dist.getLength() < cordLength) {
        spring  = spring * 0;
    }
    
    auto forces = weight + spring;
    // forces += vel * -0.04f;
    auto acc = forces * (1/mass);
    vel += acc * dt;
}


void render(Canvas& canvas) 
{
    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
    drawFilledCircle(canvas.renderer, eqPoint, 3);
    SDL_RenderDrawLineF(canvas.renderer, eqPoint.x, eqPoint.y, bjPoint.x, bjPoint.y);

    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0x00, 0x00, 0xff);
    drawFilledCircle(canvas.renderer, bjPoint, 5);
    auto rp = bjPoint + Vector2{ 0, 20 };
    drawFilledCircle(canvas.renderer, rp, 15);
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


void drawFilledCircle(SDL_Renderer *r, const Vector2& pos, float radius)
{
    auto drawHorizontalLine = [](SDL_Renderer* renderer, int x1, int x2, int y) -> void {
        for (int x = x1; x <= x2; x++)
            SDL_RenderDrawPoint(renderer, x, y);
    };
    
    int x = 0;
    int y = radius;
    int d = 3 - int(radius) << 1;

    while (y >= x) {
        // Draw horizontal lines (scanlines) for each section of the circle
        drawHorizontalLine(r, pos.x - x, pos.x + x, pos.y - y);
        drawHorizontalLine(r, pos.x - x, pos.x + x, pos.y + y);
        drawHorizontalLine(r, pos.x - y, pos.x + y, pos.y - x);
        drawHorizontalLine(r, pos.x - y, pos.x + y, pos.y + x);

        // Update decision parameter and points
        if (d < 0) {
            d = d + (x << 2) + 6;
        } else {
            d = d + ((x - y) << 2) + 10;
            y--;
        }
        x++;
        
    }
}
