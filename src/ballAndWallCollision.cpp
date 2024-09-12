#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include <cmath>

#include <SDL.h>
#include "../include/phy/Ball.h"
#include "../include/phy/Vector.h"
#include "../include/phy/Wall.h"


std::vector<phy::Ball> balls;
std::vector<phy::Wall> walls;


struct Canvas {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event evt;
    int w;
    int h;
};

float mx, my;

void init() {
    phy::Ball b1;
    b1.pos = phy::Vector2(80, 0);
    balls.push_back(b1);

    b1.pos = phy::Vector2(180, 0);
    balls.push_back(b1);

    mx = b1.mass * b1.vel.x;
    my = b1.mass * b1.vel.y;

    phy::Wall wall1;
    wall1.start = phy::Vector2(50, 250);
    wall1.end = phy::Vector2(400, 350);
    walls.push_back(wall1);
}

void update(float dt, Canvas& canvas) {

    for(auto& ball: balls) {
        ball.pos += ball.vel * dt;

        // check for boundary collision and resolve
        if(ball.pos.y + ball.radius > canvas.h) {
            ball.pos.y = canvas.h - ball.radius;
            ball.vel.y *= -1.0f;
        }

        if(ball.pos.x - ball.radius < 0) {
            ball.pos.x = ball.radius;
            ball.vel.x *= -1.0f;
        } else if(ball.pos.x + ball.radius > canvas.w) {
            ball.pos.x = canvas.w - ball.radius;
            ball.vel.x *= -1.0f;
        }

        // check for ball and wall collision
        for(const auto& wall: walls) {
            auto wallDir = wall.end - wall.start;
            auto wallDir1 = wall.start - ball.pos;
            auto wallDir2 = wall.end - ball.pos;

            auto wallDirLength = wallDir.getLength();
            auto proj1 = wallDir.dotProduct(wallDir1) / wallDirLength;
            auto proj2 = wallDir.dotProduct(wallDir2) / wallDirLength;

            auto wallDir3 = phy::Vector2(wallDir).normalize() * -proj1;
            auto dist = wallDir3 + wallDir1;

            bool test = (std::abs(proj1) < wallDirLength) && (std::abs(proj2) < wallDirLength);
            if(dist.getLength() < ball.radius && test) {
                phy::Vector2 normal{ wallDir.y, -wallDir.x };
                normal = normal.normalize();

                if(normal.dotProduct(ball.vel) > 0) {
                    normal *= -1;
                }

                float angle = ball.vel.angleBetween(wallDir);
                float displ = (ball.radius + dist.dotProduct(normal)) / std::sin(angle);
                
                auto nvel = phy::Vector2(ball.vel).normalize() * -displ;
                ball.pos += nvel;

                ball.vel.y *= -1;
            }

        }

        const float g = 3.8f;
        phy::Vector2 weight{ 0.0f, ball.mass * g };
        auto force = weight;
        auto acc = force * (1 / ball.mass);
        ball.vel += acc * dt;
    }

}

void render(Canvas& canvas) {
    for(auto& ball: balls) 
        ball.render(canvas.renderer);
    
    for(auto& wall: walls)
        wall.render(canvas.renderer);
}


void mainLoop(Canvas& canvas) {
    bool shouldClose = false;

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!shouldClose)
    {
        SDL_PollEvent(&canvas.evt);
        if(canvas.evt.type == SDL_QUIT) 
            shouldClose = true;

        SDL_SetRenderDrawColor(canvas.renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(canvas.renderer);
        render(canvas);

        auto now = std::chrono::high_resolution_clock::now();
        auto dt = (now - lastTime).count() * 10e-9;
        lastTime = now;

        update(dt, canvas);    //@todo make legit timestep

        SDL_RenderPresent(canvas.renderer);
    }
    
}


bool initCanvas(Canvas& canvas, int w, int h) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL failed to initialize" << std::endl;
        return false;
    }

    canvas.window = SDL_CreateWindow("Ball and Wall Collision", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
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
    if(!initCanvas(cnv, 600, 500)) return -1;
    init();
    mainLoop(cnv);
    return 0;
}