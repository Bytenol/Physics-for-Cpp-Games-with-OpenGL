/**
 * @file inclinedRoll.cpp
 * @brief a simple animation of a rigidbody rolling down an inclined plane
 * @date 8th, oct 2024
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
float radius = 30;
float cs = 0.4f;    // coefficient of static friction
float ck = 0.2f;    // coefficient of dynamic friction


void drawStrokedCircle(SDL_Renderer *renderer, Vector2& pos, float radius);


void init(const Canvas& cnv) {
    floorStart = Vector2{ 50.0f, 220.0f };
    floorEnd = Vector2{ 500, 250 };

    rigidBody.vertices.push_back({-radius * 0.8f, 0});
    rigidBody.vertices.push_back({radius * 0.8f, 0});
    rigidBody.vertices.push_back({0, -radius * 0.8f});
    rigidBody.vertices.push_back({0, radius * 0.8f});
    
    rigidBody.pos.x = floorStart.x;
    rigidBody.pos.y = floorStart.y - radius;
    rigidBody.im = 0.4 * rigidBody.mass * radius * radius;
}


void update(float dt, Canvas& canvas) 
{
    rigidBody.pos += rigidBody.vel * dt;
    rigidBody.angDispl = rigidBody.angVel * dt;

    for(auto it = rigidBody.vertices.begin(); it != rigidBody.vertices.end(); it++)
        *it = it->rotate(rigidBody.angDispl);

    float mg = rigidBody.mass * 20.0f;
    Vector2 weight{ 0, mg };

    Vector2 wallDir = (floorEnd - floorStart).normalize();
    float wallAngle = std::atan2(wallDir.y, wallDir.x);

    Vector2 normal = wallDir.perp(mg * std::cos(wallAngle));

    float coeff = mg * std::sin(wallAngle) / (1 + rigidBody.mass * radius * radius / rigidBody.im);

    if(coeff > cs * normal.getLength()) {
        coeff = ck * normal.getLength();
    }

    Vector2 friction = rigidBody.vel.copy().normalize() * (-coeff);
    Vector2 forces = weight + normal + friction;
    Vector2 acc = forces * (1 / rigidBody.mass);
    rigidBody.vel += acc * dt;

    float torque = friction.getLength() * radius;
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
    }

    drawStrokedCircle(canvas.renderer, rigidBody.pos, radius);

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

        update(dt, canvas);

        SDL_RenderPresent(canvas.renderer);
    }
    
}


bool initCanvas(Canvas& canvas, int w, int h) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL failed to initialize" << std::endl;
        return false;
    }

    canvas.window = SDL_CreateWindow("Inclined Roll", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
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
