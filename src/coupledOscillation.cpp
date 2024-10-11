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

Vector2 eqPoint;

struct StringPoint
{
    Vector2 pos;
    Vector2 vel;
};

std::vector<StringPoint> springPoints;


void init(const Canvas& cnv) 
{
    eqPoint.x = cnv.w * 0.5f;
    eqPoint.y = 50;

    StringPoint curr;
    curr.pos.x = eqPoint.x;
    curr.pos.y = eqPoint.y;

    for(size_t i = 0; i < 6; i++) {
        curr.pos.x = eqPoint.x + 60 * i;
        curr.pos.y = eqPoint.y;//100 + 60 * i;
        springPoints.push_back(curr);
    }
}

float a = 0.0f;

void update(float dt, Canvas& canvas) 
{
    float kSpring = 5.0f;
    float mass = 2;
    float L = 50;
    
    a += 0.01f;
    eqPoint.x = canvas.w * 0.5 + std::sin(a) * 100;

    for(int i = 0; i < springPoints.size(); i++) {
        auto& curr = springPoints[i];
        curr.pos += curr.vel * dt; 

        float dampingCoeff = 0.5f;
        Vector2 damping = curr.vel * (curr.vel.getLength() > 0 ? -dampingCoeff : 0);

        auto centerPrev = i <= 0 ? eqPoint : springPoints[i - 1].pos;
        auto centerNext = (i < springPoints.size() - 1 ? springPoints[i+1] : springPoints.back()).pos;

        auto displPrev = curr.pos -centerPrev;
        auto displNext = curr.pos - centerNext;

        auto extensionPrev = displPrev - (displPrev.copy().normalize() * L);
        auto extensionNext = displNext - (displNext.copy().normalize() * L);

        auto restoringPrev = extensionPrev * -kSpring;
        auto restoringNext = extensionNext * -kSpring;

        Vector2 weight{ 0, mass * 10 };
        
        Vector2 force = weight + damping + restoringPrev + restoringNext;
        Vector2 acc = force * (1 / mass);
        curr.vel += acc * dt;
    }
}


void render(Canvas& canvas) 
{
    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
    drawFilledCircle(canvas.renderer, eqPoint, 3);

    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLineF(canvas.renderer, eqPoint.x, eqPoint.y, springPoints[0].pos.x, springPoints[0].pos.y);

    for(size_t i = 0; i < springPoints.size(); i++) {
        const auto& curr = springPoints[i];
        const auto& next = springPoints[i + 1 >= springPoints.size() ? i : i + 1];
        SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawLineF(canvas.renderer, curr.pos.x, curr.pos.y, next.pos.x, next.pos.y);
        SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0x00, 0x00, 0xff);
        drawFilledCircle(canvas.renderer, curr.pos, 10);
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

    canvas.window = SDL_CreateWindow("Coupled Oscillation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
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
