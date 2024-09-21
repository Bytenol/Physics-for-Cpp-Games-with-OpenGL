/**
 * @todo erase the error in the console
 * 
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
    body.pos.y = 300;
    lastTime = std::chrono::high_resolution_clock::now();
}


void update(float dt, Canvas& cnv)
{

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