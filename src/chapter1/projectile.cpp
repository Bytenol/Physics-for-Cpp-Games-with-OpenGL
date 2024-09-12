/**
 * @file projectile.cpp
 * @date 14 Aug, 2024
 * This program demonstrates the use of analytical and numerical
 * solutions to move a ball in a projectile motion. The analytical
 * procedure used is based on the equation of motion for displacement
 * 
 * S = ut + (1/2)at^2
 * 
 * While the numerical procedure uses the Euler's scheme
 * S += v * t;
 */
#include <chrono> 
#include <vector>
#include "../../include/phy/phy.h"

using namespace phy;
using namespace std::chrono;


int main(int, char**){
    const int W = 500;
    const int H = 250;
    auto ctx = createContext(W, H, "Balls");
    if(!ctx) return -1;

    Ball ball;
    ball.radius = 10.0f;
    ball.pos = Vector(ball.radius * 1.5f, H - ball.radius);
    ball.vel = Vector(30, -100);

    Vector acc{0.0f, 50.0f};   // gravity

    auto previousTime = high_resolution_clock::now();
    auto currentTime = previousTime;

    while (!glfwWindowShouldClose(ctx->getWindow()))
    {
        glfwPollEvents();
        currentTime = high_resolution_clock::now();
        float dt = (static_cast<duration<float>>(currentTime - previousTime)).count();
        previousTime = currentTime;

        // ball.pos += ball.vel * dt;
        // ball.vel += acc * dt;
        ball.pos += ball.vel * dt + (acc * dt * dt * 0.5f); // ut + 1/2at^2
        ball.vel += acc * dt;

        if(ball.pos.y > ctx->getHeight() - ball.radius) {
            ball.pos.y = ctx->getHeight() - ball.radius;
            ball.vel.y *= -0.8;
        }

        if(ball.pos.x > ctx->getWidth() + ball.radius) {
            ball.pos.x = -ball.radius;
        }

        ctx->resetMatrix();
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        ball.draw(ctx);
        glfwSwapBuffers(ctx->getWindow());
    }
    
}