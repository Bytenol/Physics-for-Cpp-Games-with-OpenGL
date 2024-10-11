/**
 * @file rope.cpp
 * @brief a simple 
 * @date 11th Oct, 2024
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

struct SpringMass
{
    Vector2 pos;
    Vector2 vel;
};

float mass = 0.5;
float g = 10;
float L = 2;
float kSpring = 0.5f;
Vector2 eqPoint1, eqPoint2;
std::vector<SpringMass> springPoints;


void init(const Canvas& cnv) 
{
    eqPoint1.x = 50;
    eqPoint1.y = 50;
    eqPoint2.x = cnv.w - 50;
    eqPoint2.y = 80;

    int div = (eqPoint2.x - eqPoint1.x) / 10;

    for(size_t i = 1; i <= 9; i++) {
        Vector2 pos;
        pos.x = (i + 1) * div;
        pos.y = eqPoint1.y;
        SpringMass pt{ pos };
        springPoints.push_back(pt);
    }

}


void update(float dt, Canvas& canvas) 
{

    for(int i = 0; i < springPoints.size(); i++) {
        auto& body = springPoints[i];
        body.pos += body.vel * dt;

        auto lastPos = (i == 0 ? eqPoint1 : springPoints[i - 1].pos);
        auto nextPos = (i + 1 >= springPoints.size() ? eqPoint2 : springPoints[i + 1].pos);

        auto lastDispl = body.pos - lastPos;
        auto nextDispl = body.pos - nextPos;

        auto restoringLast = (lastDispl - (lastDispl.copy().normalize() * L)) * -kSpring; 
        auto restoringNext = (nextDispl - (nextDispl.copy().normalize() * L)) * -kSpring; 

        auto v1 = (body.vel * 2) - (i == 0 ? body.vel : springPoints[i - 1].vel) - (i + 1 >= springPoints.size() ? springPoints[i].vel : springPoints[i + 1].vel);
        Vector2 damping = v1 * -0.9;

        Vector2 weight{ 0, mass * g };
        Vector2 forces = weight + restoringLast + restoringNext + damping;
        Vector2 acc = forces * (1 / mass);
        body.vel += acc * dt;
    }
}


void render(Canvas& canvas) 
{
    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
    drawFilledCircle(canvas.renderer, eqPoint1, 3);
    drawFilledCircle(canvas.renderer, eqPoint2, 3);
    SDL_RenderDrawLineF(canvas.renderer, eqPoint1.x, eqPoint1.y, springPoints.front().pos.x,springPoints.front().pos.y);
    SDL_RenderDrawLineF(canvas.renderer, eqPoint2.x, eqPoint2.y, springPoints.back().pos.x,springPoints.back().pos.y);

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
            if(canvas.evt.type == SDL_MOUSEMOTION) {
                eqPoint2.x = canvas.evt.motion.x;
                eqPoint2.y = canvas.evt.motion.y;
            }
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

    canvas.window = SDL_CreateWindow("Rope", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
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
