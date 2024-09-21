/**
 * @todo add normal force when the block is lying flat
 * @todo erase the error in the browser console
 * @todo remove html text area
 * @todo fix timestep : (too fast now)
 * @todo add proper forces when it's lying down
 * @todo handle when tab switches
 * @todo set size as window size
 */
#include <iostream>
#include <chrono>
#include <SDL/SDL.h>
#include <emscripten/emscripten.h>
#include <phy/RigidBody.h>

struct Canvas 
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event evt;
    int w;
    int h;

} canvas;

bool initCanvas(Canvas& canvas, int w, int h);
void init(Canvas& cnv);
void mainLoop();
void render(Canvas& cnv);
void update(float dt, Canvas& cnv);

decltype(std::chrono::high_resolution_clock::now()) lastTime; 

phy::RigidBody body;
phy::Vector2 acc, force;
float torque, alp;
const int floorY = 550;

int main(int argc, char* argv[])
{
    if(!initCanvas(canvas, 500, 600))
        return -1;
    init(canvas);
    emscripten_set_main_loop(mainLoop, 0, 1);
    return 0;
}


void init(Canvas& cnv)
{
    body = phy::RigidBody({
        { -50.0f, -25.0f },
        { 50.0f, -25.0f },
        { 50.0f, 25.0f },
        { -50.0f, 25.0f },
    });
    body.pos.x = 300;
    body.pos.y = 0;
    body.mass = 1;
    body.im = 10000;
    body.setRotation(45 * 3.14159f / 180);
    lastTime = std::chrono::high_resolution_clock::now();
}


void update(float dt, Canvas& cnv)
{
    body.pos += body.vel * dt;
    body.setRotation(body.angVel * dt);

    // check for collisions
    for(auto it = body.vertices.begin(); it != body.vertices.end(); it++)
    {
        auto crossProduct = [](const phy::Vector2& a, const phy::Vector2& b) {
            return a.x * b.y - a.y * b.x;
        };

        const auto nPos = body.pos + it->rotate(body.getRotation());
        if(nPos.y >= floorY)
        {
            body.pos.y -= (nPos.y - floorY);
            auto rp1 = it->rotate(body.getRotation());
            auto g = phy::Vector2(rp1.y, -rp1.x).normalize() * (-body.angVel * rp1.getLength());
            auto vp1 = body.vel + g;
            auto normal = phy::Vector2(0, -1);
            auto rp1Xnormal = crossProduct(rp1, normal);
            const float cr = 0.4f;
            const float impulse = -(1+cr)*vp1.dotProduct(normal)/(1/body.mass + rp1Xnormal*rp1Xnormal/body.im); 
            body.vel += normal * (impulse/body.mass);
            body.angVel += rp1Xnormal * impulse/body.im;
        }
    }

    // forces and torque
    const auto weight = phy::Vector2(0.0f, body.mass * 10.0f);
    force = weight;
    torque = 0;
    torque += -0.4f * body.angVel;

    acc = force * (1/ body.mass);
    alp = torque / body.im;
    body.vel += acc * dt;
    body.angVel += alp * dt;
}


void render(Canvas& canvas) 
{
    // draw polygons
    SDL_SetRenderDrawColor(canvas.renderer, 255, 0, 0, 255);
    const int maxSize = body.vertices.size();
    for(int i = 0; i < maxSize; i++)
    {
        const auto& v = body.vertices[i];
        const auto p1 = body.pos + body.vertices[i];
        const auto p2 = body.pos + body.vertices[(i + 1 >= maxSize ? 0 : i + 1)];
        SDL_RenderDrawLine(canvas.renderer, p1.x, p1.y, p2.x, p2.y);
    }

    SDL_SetRenderDrawColor(canvas.renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(canvas.renderer, 0, floorY, 500, floorY);
}


void mainLoop() 
{
    SDL_PollEvent(&canvas.evt);

    SDL_SetRenderDrawColor(canvas.renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(canvas.renderer);
    render(canvas);
    SDL_RenderPresent(canvas.renderer);

    auto now = std::chrono::high_resolution_clock::now();
    auto dt = (now - lastTime).count() * 10e-9;
    lastTime = now;

    update(dt, canvas);
    
}


bool initCanvas(Canvas& canvas, int w, int h) 
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cerr << "SDL failed to initialize" << std::endl;
        return false;
    }

    canvas.window = SDL_CreateWindow("RigidBody", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    if(!canvas.window) 
    {
        std::cerr << "SDL failed to create window" << std::endl;
        return false;
    }
    canvas.w = w;
    canvas.h = h;

    canvas.renderer = SDL_CreateRenderer(canvas.window, 0, SDL_RENDERER_ACCELERATED);
    if(!canvas.renderer) 
    {
        std::cerr << "SDL failed to create renderer" << std::endl;
        return false;
    }

    return true;
}