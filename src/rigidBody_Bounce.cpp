/**
 * @file rigidBody_Bounce.cpp
 * @date 17th Sept, 2024 // Tuesday => (LEFT ADRIN)
 * @brief This program simulate the reaction of rigid bodies bouncing of walls
 * 
 * Modified: 8th oct, 2024
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

bool shouldRun = true;

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
    floorStart = Vector2{ 0.0f, 470 };
    floorEnd = Vector2{ (float)cnv.w, 470 };

    rigidBody.vertices.push_back({-50, -25});
    rigidBody.vertices.push_back({50, -25});
    rigidBody.vertices.push_back({50, 25});
    rigidBody.vertices.push_back({-50, 25});

    rigidBody.pos.x = cnv.w / 2;
    rigidBody.pos.y = 0;

    rigidBody.angVel = 0.2f;
}


void update(float dt, Canvas& canvas) 
{
    rigidBody.pos += rigidBody.vel * dt;
    rigidBody.angDispl = rigidBody.angVel * dt;

    for(auto it = rigidBody.vertices.begin(); it != rigidBody.vertices.end(); it++)
        *it = it->rotate(rigidBody.angDispl);

    for(auto it = rigidBody.vertices.begin(); it != rigidBody.vertices.end(); it++)
    {
        auto transformed = rigidBody.pos + *it;
        if(transformed.y >= floorEnd.y) {
            rigidBody.pos.y -= (transformed.y - floorEnd.y);
            auto& rp1 = *it;
            Vector2 vp1 = rigidBody.vel + rp1.perp(-rigidBody.angVel * rp1.getLength());
            Vector2 normal{ 0, -1 };
            float rp1Xnormal = rp1 * normal;
            float impulse = -(1+0.4)*vp1.dotProduct(normal)/(1/rigidBody.mass + rp1Xnormal*rp1Xnormal/rigidBody.im);
            rigidBody.vel = rigidBody.vel + (normal * (impulse/rigidBody.mass));
            rigidBody.angVel += rp1Xnormal * impulse/rigidBody.im;
        }
    }

    float mg = rigidBody.mass * 10.0f;
    Vector2 weight{ 0, mg };

    Vector2 forces = weight;
    Vector2 acc = forces * (1 / rigidBody.mass);
    rigidBody.vel += acc * dt;

    float torque = 0;
    torque += -rigidBody.angVel * 0.4;
    float alph = torque / rigidBody.im;
    rigidBody.angVel += alph * dt;
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

    SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderDrawLineF(canvas.renderer, floorStart.x, floorStart.y, floorEnd.x, floorEnd.y);

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

        if(shouldRun) update(dt, canvas);

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