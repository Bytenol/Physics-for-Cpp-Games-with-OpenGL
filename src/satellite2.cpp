/**
 * @file Satellite2.cpp
 * @brief a simple implementation of a satellite rotating around the earth
 * @date 30th Sept, 2024
 * 
 * This file shows the implementation of circular rotational motion whereby a satellite 
 * rotates freely around the earth
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

const float G = 400;
Vector2 earthPos, satPos;
Vector2 satVel;
Vector2 fGravity;
std::vector<Vector2> satVertices;
float earthMass, satMass;
float earthRadius, satRadius;
float angDisp, angVel;

void drawFilledCircle(SDL_Renderer *r, const Vector2& pos, float radius);

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
    earthMass = 5;

    satPos = earthPos + Vector2{ 1, 0 } * (earthRadius * 2.5);
    satRadius = 10;
    satMass = earthMass * 0.3;

    auto diff = earthPos - satPos;
    float dist = diff.getLength();
    satVel = Vector2{ 0, -1 } * std::sqrt(G * earthMass / dist);
    angVel = std::sqrt(G * earthMass / (dist * dist * dist));

    // vertex points for satellite
    for(size_t i = 0; i < 360; i++)
    {
        float a = i * 3.14159f / 180;
        float px = std::cos(a) * satRadius;
        float py = std::sin(a) * satRadius;
        satVertices.push_back({ px, py });
    }
}


void update(float dt, Canvas& canvas) 
{
    satPos += satVel * dt;
    angDisp += angVel * dt;

    auto diff = earthPos - satPos;
    float dist = diff.getLength();
    float gMag = (G * earthMass * satMass) / (dist * dist);
    fGravity = diff.copy().normalize() * gMag;

    auto force = fGravity;
    auto acc = force * (1 / satMass);
    satVel += acc * dt;
}


void render(Canvas& canvas) 
{
    SDL_SetRenderDrawColor(canvas.renderer, 0x00, 0x00, 0xff, 0xff);
    drawFilledCircle(canvas.renderer, earthPos, earthRadius);

    // draw satellite
    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0x00, 0x00, 0xff);
    for(auto& vertex: satVertices)
    {
        auto&& transformed = satPos + vertex;
        SDL_RenderDrawPoint(canvas.renderer, int(transformed.x), int(transformed.y));
    }

    for(size_t i = 0; i < 4; i++)
    {
        const float a = i * 90 * 3.14159f / 180;
        Vector2 p = satPos + Vector2{ std::cos(a), std::sin(a) }.rotate(angDisp) * satRadius;
        SDL_RenderDrawLine(canvas.renderer, satPos.x, satPos.y, p.x, p.y);
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
