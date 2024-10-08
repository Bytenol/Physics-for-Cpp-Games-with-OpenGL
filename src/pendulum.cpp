/**
 * @file pendulum.cpp
 * @brief a simple animation of a non-uniform circular motion
 * @date 6th, oct 2024
 * 
 */

#include <iostream>
#include <chrono>
#include <cmath>

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


float angle = 30.0f * 3.14159f / 180;   // in radians
float g = 10.0f;
float mass = 5;    // in grams
Vector2 tension;
Vector2 weight;
Vector2 origin;
Vector2 pos, vel;


void init(const Canvas& cnv) {
    origin.x = cnv.w * 0.5;
    origin.y = cnv.h * 0.1;
    pos.x = origin.x + std::sin(angle) * 100;
    pos.y = origin.y + std::cos(angle) * 100;
    vel.x = 0;
    vel.y = 0;
}


void update(float dt, Canvas& canvas) 
{
    pos += vel * dt;

    weight.x = 0.0f;
    weight.y = g * mass;

    auto diffPos = (origin - pos);
    float l = diffPos.getLength();
    float esc = l - 100;
    pos += diffPos.copy().normalize() * esc;
    angle =  std::atan2(diffPos.y, diffPos.y);
    tension.y = -mass * g * std::cos(90 * 3.14159f / 180 - angle);
    tension.x = mass * g * std::sin( 90 * 3.14159f / 180 - angle);
    // if(angle > 1.5) tension.x *= -1;

    auto force = tension + weight;
    auto acc = force * (1 / mass);
    vel += acc * dt;
}


void render(Canvas& canvas) 
{
    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
    drawFilledCircle(canvas.renderer, origin, 2);
    SDL_RenderDrawLineF(canvas.renderer, origin.x, origin.y, pos.x, pos.y);

    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0x00, 0x00, 0xff);
    drawFilledCircle(canvas.renderer, pos, 10);
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

    canvas.window = SDL_CreateWindow("Pendulum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
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