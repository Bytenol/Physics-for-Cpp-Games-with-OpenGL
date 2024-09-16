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

void init() {
    phy::Ball b1;
    b1.pos = phy::Vector2(80, 200);
    b1.vel.x = 20;
    b1.vel.y = 30;
    balls.push_back(b1);

    b1 = phy::Ball(50, 1, 0x00ff00);
    b1.pos = phy::Vector2(300, 200);
    b1.vel.x = -5;
    b1.vel.y = -30;
    balls.push_back(b1);


    for(int i = 0; i < 3; i++) {
        for(int j= 0; j < 5; j++) {
            b1.pos.y = (i - j) - 30;
            b1.pos.x = j;
            // balls.push_back(b1);
        }
    }

    phy::Wall wall1;
    wall1.start = phy::Vector2(50, 250);
    wall1.end = phy::Vector2(400, 350);
    // walls.push_back(wall1);
}

void update(float dt, Canvas& canvas) {

    for(auto it = balls.begin(); it != balls.end(); it++) {
        auto& ball = it;
        ball->pos += ball->vel * dt;

         // check for boundary collision and resolve
        if(ball->pos.y + ball->radius > canvas.h) {
            ball->pos.y = canvas.h - ball->radius;
            ball->vel.y *= -1.f;
        }

        if(ball->pos.x - ball->radius < 0) {
            ball->pos.x = ball->radius;
            ball->vel.x *= -1.f;
        } else if(ball->pos.x + ball->radius > canvas.w) {
            ball->pos.x = canvas.w - ball->radius;
            ball->vel.x *= -1.f;
        }


        for(auto it2 = it + 1; it2 != balls.end(); it2++) {
            auto& ball2 = it2;

            auto dist = ball2->pos - ball->pos;
            const float totalRadius = ball->radius + ball2->radius;
            const float distLength = dist.getLength();

            if(distLength < totalRadius) {
                const float overlap = totalRadius - distLength;
                const auto normal = phy::Vector2(dist).normalize();
                const auto tangent = phy::Vector2(normal.y, -normal.x);

                const float u1 = ball->vel.dotProduct(normal);
                const float u2 = ball2->vel.dotProduct(normal);

                auto normalVel1 = normal * u1;
                auto normalVel2 = normal * u2;

                const auto tangentVel1 = ball->vel - normalVel1;
                const auto tangentVel2 = ball2->vel - normalVel2;

                ball->pos -= normal * overlap * 0.5f;
                ball2->pos += normal * overlap * 0.5f;

                float m1 = ball->mass;
                float m2 = ball2->mass;
                float v1 = ((m1 - m2) * u1 + 2 * m2 * u2) / (m1 + m2);
                float v2 = ((m2 - m1) * u2 + 2 * m1 * u1) / (m1 + m2);

                normalVel1 = normal * v1;
                normalVel2 = normal * v2;

                ball->vel = tangentVel1 + normalVel1;
                ball2->vel = tangentVel2 + normalVel2;
            }

        }

        // check for ball and wall collision
        for(const auto& wall: walls) {
            auto wallDir = wall.end - wall.start;
            auto wallDir1 = wall.start - ball->pos;
            auto wallDir2 = wall.end - ball->pos;

            auto wallDirLength = wallDir.getLength();
            auto proj1 = wallDir.dotProduct(wallDir1) / wallDirLength;
            auto proj2 = wallDir.dotProduct(wallDir2) / wallDirLength;

            auto wallDir3 = phy::Vector2(wallDir).normalize() * -proj1;
            auto dist = wallDir3 + wallDir1;

            bool test = (std::abs(proj1) < wallDirLength) && (std::abs(proj2) < wallDirLength);
            if(dist.getLength() < ball->radius && test) {
                phy::Vector2 normal{ wallDir.y, -wallDir.x };
                normal = normal.normalize();

                if(normal.dotProduct(ball->vel) > 0) {
                    normal *= -1;
                }

                float angle = ball->vel.angleBetween(wallDir);
                float displ = (ball->radius + dist.dotProduct(normal)) / std::sin(angle);
                
                auto nvel = phy::Vector2(ball->vel).normalize() * -displ;
                ball->pos += nvel;
                ball->pos.y -= 0.1f;
                
                float friction = -1;
                float normalVelProj = ball->vel.dotProduct(dist) / dist.getLength();
                auto normalVel = phy::Vector2(dist).normalize() * normalVelProj * friction;
                auto tangentVel = ball->vel + normalVel;

                ball->vel = normalVel + tangentVel;
            }

        }


        const float g = 3.8f;
        phy::Vector2 weight{ 0.0f, ball->mass * g };
        auto force = weight;
        auto acc = force * (1 / ball->mass);
        ball->vel += acc * dt;
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

        update(dt, canvas);

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