/**
 * @file ballAndWallCollision.cpp
 * @date sept 09, 2024
 */
#include <iostream>

#include "../../include/phy/phy.h"

const float g = 20.0f;
phy::Ball ball;
phy::Vector weight;

struct Wall {
    phy::Vector start;
    phy::Vector end;
} wall;


void init(phy::CanvasElementPtr& ctx) {
    ball.pos = phy::Vector(90.0f, 0.0f);
    ball.vel = phy::Vector(0, 0);

    wall.start = phy::Vector(80.0f, 100.0f);
    wall.end = phy::Vector(500.0f, 400.0f);
    
    weight.x = 0.0f;
    weight.y = ball.mass * g;
}


void update(float dt) {
    ball.pos += ball.vel * dt;

    auto force = weight;
    auto acc = force * (1/ball.mass);
    ball.vel += acc * dt;
}

void render(phy::CanvasElementPtr& ctx) {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    ball.draw(ctx);
    ctx->setFillColor(0x000);
    ctx->drawLine(0.0f, 0.0f, 300.0f, 300.0f);
    std::cout << 1 << std::endl;
}

void mainLoop(phy::CanvasElementPtr& ctx) {
    while(!glfwWindowShouldClose(ctx->getWindow())) {
        glfwPollEvents();
        ctx->resetMatrix();
        render(ctx);
        //@todo: make dt dynamic
        update(1/60.0f);
        glfwSwapBuffers(ctx->getWindow());
    }
}

int main()
{
    auto ctx = phy::createContext(700, 600, "Ball and Wall collision");
    if(!ctx) return -1;
    init(ctx);
    mainLoop(ctx);
    return 0;
}
