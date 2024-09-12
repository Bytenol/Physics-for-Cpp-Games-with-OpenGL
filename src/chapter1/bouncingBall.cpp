/**
 * @file bouncingBall.cpp
 * @date 14 Aug, 2024
 * This program demonstrates the physics of a ball falling 
 * under the influence of gravitational acceleration, and then 
 * bounces off the floor while losing some of it's vertical speed
 */
#include "../../include/phy/phy.h"

using namespace phy;


int main(int, char**){
    auto ctx = createContext(400, 500, "Bouncing ball");
    if(!ctx) return -1;

    float radius = 20.0f;
    uint color = 0x0000ff;
    float g = 0.1f;     // acceleration due to gravity
    float x = 50.0f;    // position on the x-axis
    float y = 50.0f;    // position on the y-axis
    float vx = 2;       // initial horizontal speed
    float vy = 0;       // initial vertical speed

    
    while (!glfwWindowShouldClose(ctx->getWindow()))
    {
        glfwPollEvents();
        ctx->resetMatrix();
        vy += g;
        x += vx;
        y += vy;

        if(y > ctx->getHeight() - radius) {
            y = ctx->getHeight() - radius;
            vy *= -0.8;
        }

        if(x > ctx->getWidth() + radius) {
            x = -radius;
        }

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        ctx->setFillColor(color);
        ctx->drawArc(x, y, radius);
        glfwSwapBuffers(ctx->getWindow());
    }
    
}