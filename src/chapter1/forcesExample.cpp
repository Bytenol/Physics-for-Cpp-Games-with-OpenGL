/**
 * @file forcesExample.cpp
 * @date 14 Aug, 2024
 * This program demonstrates the effect of gravity and drag force
 * on a falling object. It can be observed that the ball will attain
 * a terminal velocity at a point in time whereby the velocity is 
 * just constant. 
 * 
 * The condition for a terminal velocity to be attained is met when the forces
 * drag = weight, i.e
 * kv = mg => (v = (mg)/k)
 */
#include <chrono>
#include "../../include/phy/phy.h"

using namespace phy;
using namespace std::chrono;


int main(int, char**){
    auto ctx = createContext(400, 500, "Forces Example");
    if(!ctx) return -1;

    Ball ball;
    ball.radius = 10.0f;
    ball.pos = Vector(200, 0);

    const float g = 100.0f; // acceleration due to gravity
    const float k = 0.5f;   // drag coefficient
    Vector weight, drag, acc;

    auto previousTime = high_resolution_clock::now();
    auto currentTime = previousTime;

    while (!glfwWindowShouldClose(ctx->getWindow()))
    {
        currentTime = high_resolution_clock::now();
        float dt = (static_cast<duration<float>>(currentTime - previousTime)).count();
        previousTime = currentTime;

        glfwPollEvents();
        ctx->resetMatrix();

        ball.pos += ball.vel * dt;

        // calculate acceleration
        weight.y = ball.mass * g;
        drag.y = ball.vel.y * -k;
        acc = (weight + drag) * (1/ball.mass);

        std::cout << ball.vel.y << std::endl;

        if(ball.pos.y > ctx->getHeight() - ball.radius) {
            ball.pos.y = ctx->getHeight() - ball.radius;
            acc.y = 0.0f;
            ball.vel.y *= -0.8f;
        }

        if(ball.pos.x > ctx->getWidth() + ball.radius) {
            ball.pos.x = -ball.radius;
        }

        ball.vel += acc * dt;

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        ball.draw(ctx);
        glfwSwapBuffers(ctx->getWindow());
    }
    
}