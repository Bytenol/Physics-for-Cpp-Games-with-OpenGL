/**
 * @file frameExample.cpp
 * @date 14 Aug, 2024
 * This program utilizes delta time to demonstrates the physics of a ball falling 
 * under the influence of gravitational acceleration, and then 
 * bounces off the floor while losing some of it's vertical speed.
 */
#include <chrono>
#include "../../include/phy/phy.h"

using namespace phy;
using namespace std::chrono;

struct Ball {
    float x, y, radius,
        vx, vy;

    Ball(float x, float y, float r) {
        this->x = x;
        this->y = y;
        radius = r;
        vx = 0.0f;
        vy = 0.0f;
    }

    void draw(CanvasElementPtr& ctx) {
        ctx->setFillColor(0x0000ff);
        ctx->drawArc(x, y, radius);
    }

};


int main(int, char**){
    auto ctx = createContext(400, 500, "Frame Example");
    if(!ctx) return -1;

    float g = 200.0f;     // acceleration due to gravity
    ::Ball ball(50.0f, 50.0f, 20.0f);
    ball.vx = 20.0f;

    auto previousTime = high_resolution_clock::now();
    auto currentTime = previousTime;

    while (!glfwWindowShouldClose(ctx->getWindow()))
    {
        glfwPollEvents();
        currentTime = high_resolution_clock::now();
        float dt = (static_cast<duration<float>>(currentTime - previousTime)).count();
        previousTime = currentTime;

        ball.vy += g * dt;
        ball.x += ball.vx * dt;
        ball.y += ball.vy * dt;

        if(ball.y > ctx->getHeight() - ball.radius) {
            ball.y = ctx->getHeight() - ball.radius;
            ball.vy *= -0.8f;
        }

        if(ball.x > ctx->getWidth() + ball.radius) {
            ball.x = -ball.radius;
        }

        ctx->resetMatrix();
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        ball.draw(ctx);
        glfwSwapBuffers(ctx->getWindow());
    }
    
}