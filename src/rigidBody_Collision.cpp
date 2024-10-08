/**
 * @file rigidBody_Bounce.cpp
 * @date 17th Sept, 2024 // Tuesday => (LEFT ADRIN)
 * @brief This program simulate the reaction of rigid bodies bouncing of walls
 * 
 * Modified: 8th oct, 2024
 */
#include <iostream>
#include <chrono>
#include <cmath>
#include <vector>
#include <cassert>

#include <SDL.h>
#include <phy/Vector.h>

using namespace phy;

struct RigidBody;
struct Canvas;

/// @brief Carry out seperating axis theorem algorithms on polygons
/// @param polygon The polygon to check 
/// @param polygon2 The potential polygon it will collide with
/// @return true if there is any collision
bool sat_collision(SDL_Renderer* renderer, RigidBody& b1, RigidBody& b2);

bool pointInPolyCollision(SDL_Renderer* renderer, RigidBody& b1, RigidBody& b2);

void processEvent(Canvas& cnv);

struct Canvas {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event evt;
    int w;
    int h;
};

struct Color { 
    int r = 190, 
    g = 56, 
    b = 102; 
};

// rigid body types
struct RigidBody
{
    Vector2 pos;
    Vector2 vel;
    float mass = 1.0f;
    float im = 5000;
    float angVel = 0.0f;
    float angDispl = 0.0f;
    std::vector<Vector2> vertices;
    Color color;
};

struct CollisionInfo
{
    Vector2 normal, normalStart, normalEnd;
    Color color;
};


bool shouldRun = true;
Vector2 floorStart, floorEnd;
std::vector<RigidBody> bodies;
std::vector<CollisionInfo> collisionInfo;


void init(const Canvas& cnv) {
    floorStart = Vector2{ 0.0f, 470 };
    floorEnd = Vector2{ (float)cnv.w, 470 };

    RigidBody body;
    body.vertices.push_back({-50, -25});
    body.vertices.push_back({50, -25});
    body.vertices.push_back({50, 25});
    body.vertices.push_back({-50, 25});

    body.pos.x = cnv.w / 2;
    body.pos.y = cnv.h / 2;

    // body.angVel = 0.2f;
    bodies.push_back(body);

    body.vertices.clear();
    // body.vertices.push_back({-25, -50});
    // body.vertices.push_back({25, -50});
    // body.vertices.push_back({25, 50});
    // body.vertices.push_back({-25, 50});

    body.vertices.push_back({-25, -50});
    body.vertices.push_back({25, -50});
    body.vertices.push_back({25, 50});
    // body.vertices.push_back({-25, 50});

    body.pos.x = cnv.w / 2;
    body.pos.y = 100;

    // body.angVel = 0.2f;
    bodies.push_back(body);
}


void update(float dt, Canvas& canvas) 
{
    collisionInfo.clear();

    for(int i = 0; i < bodies.size(); i++)
    {
        auto& body1 = bodies[i];

        body1.pos += body1.vel * dt;
        body1.angDispl = body1.angVel * dt;

        for(auto it = body1.vertices.begin(); it != body1.vertices.end(); it++)
            *it = it->rotate(body1.angDispl);

        // check for floor collision
        for(auto it = body1.vertices.begin(); it != body1.vertices.end(); it++)
        {
            auto transformed = body1.pos + *it;
            if(transformed.y >= floorEnd.y) {
                body1.pos.y -= (transformed.y - floorEnd.y);
                auto& rp1 = *it;
                Vector2 vp1 = body1.vel + rp1.perp(-body1.angVel * rp1.getLength());
                Vector2 normal{ 0, -1 };
                float rp1Xnormal = rp1 * normal;
                float impulse = -(1+0.4)*vp1.dotProduct(normal)/(1/body1.mass + rp1Xnormal*rp1Xnormal/body1.im);
                body1.vel = body1.vel + (normal * (impulse/body1.mass));
                body1.angVel += rp1Xnormal * impulse/body1.im;
            }
        }

        // check for polygon collision
        for(int j = i + 1; j < bodies.size(); j++)
        {
            auto& body2 = bodies[j];
            body1.color.b = 102;

            // if(sat_collision(canvas.renderer, body1, body2)) {
            //     body1.color.b = 0x00;
            // }

            if(pointInPolyCollision(canvas.renderer, body1, body2)) {
                body1.color.b = 0x00;
            }

        }

        // float mg = body1.mass * 10.0f;
        // Vector2 weight{ 0, mg };

        // Vector2 forces = weight;
        // Vector2 acc = forces * (1 / body1.mass);
        // body1.vel += acc * dt;

        float torque = 0;
        torque += -body1.angVel * 0.4;
        float alph = torque / body1.im;
        body1.angVel += alph * dt;
    }
}


bool pointInPolyCollision(SDL_Renderer* renderer, RigidBody& b1, RigidBody& b2)
{
    bool isInPath = false;

    for(int i = 0; i < b1.vertices.size(); i++) {
        auto vertex1 = b1.pos + b1.vertices[i];

        for(int j = 0; j < b2.vertices.size(); j++) {
            auto vertex2 = b2.pos + b2.vertices[i];
        }

        if(isInPath) return true;

    }

    return false;
}


bool sat_collision(SDL_Renderer* renderer, RigidBody& b1, RigidBody& b2)
{
    RigidBody* poly1 = &b1;
    RigidBody* poly2 = &b2;

    std::vector<CollisionInfo> firstCol, secondCol;
    CollisionInfo mainInfo;
    float minOverlap = INFINITY;

    for(short i = 0; i < 2; i++)
    {
        if(i > 0) {
            poly1 = &b2;
            poly2 = &b1;
        }

        for(int i = 0; i < poly1->vertices.size(); i++)
        {
            auto curr = poly1->pos + poly1->vertices[i];
            auto next = poly1->pos + poly1->vertices[(i + 1) % poly1->vertices.size()];
            auto normal = (next - curr).perp();

            float min_1 = INFINITY;
            float max_1 = -INFINITY;

            for(const auto& vertex: poly1->vertices) {
                float dp = (poly1->pos + vertex).dotProduct(normal);
                min_1 = std::min(min_1, dp);
                max_1 = std::max(max_1, dp);
            }

            float min_2 = INFINITY;
            float max_2 = -INFINITY;

            for(const auto& vertex: poly2->vertices) {
                float dp = (poly2->pos + vertex).dotProduct(normal);
                min_2 = std::min(min_2, dp);
                max_2 = std::max(max_2, dp);
            }

            if(!(min_1 <= max_2 && min_2 <= max_1))
                return false;

            float l1 = std::min(max_1, max_2) - std::max(min_1, min_2);
            if(l1 < minOverlap) {
                minOverlap = l1;
                mainInfo.normal = normal;
                mainInfo.normalStart = curr;
                mainInfo.normalEnd = mainInfo.normalStart + mainInfo.normal * 30;
                mainInfo.color.r = 0x83;
                mainInfo.color.g = i > 0 ? 0xaa : 0x86;
                mainInfo.color.b = i > 0 ? 0x97 : 0xf2;
                // if(i == 0) firstCol.push_back(mainInfo);
                // else secondCol.push_back(mainInfo);
            }
        }
    }

    // collisionInfo.insert(collisionInfo.begin(), firstCol.begin(), firstCol.end());
    // collisionInfo.insert(collisionInfo.begin(), secondCol.begin(), secondCol.end());
    collisionInfo.push_back(mainInfo);

    // auto g = (b1.pos - b2.pos).normalize() * minOverlap;
    // b1.pos += g;

    return true;
}


void render(Canvas& canvas) 
{
    for(const auto& body: bodies) 
    {
        SDL_SetRenderDrawColor(canvas.renderer, body.color.r, body.color.g, body.color.b, 0xff);
        for(size_t i = 0; i < body.vertices.size(); i++)
        {
            auto cPos = body.pos + body.vertices[i];
            auto nPos = body.pos + body.vertices[(i + 1) % body.vertices.size()];
            SDL_RenderDrawLineF(canvas.renderer, cPos.x, cPos.y, nPos.x, nPos.y);
            if(i == 0)
                SDL_RenderDrawLineF(canvas.renderer, body.pos.x, body.pos.y, cPos.x, cPos.y);
        }
    }

    for(const auto& info: collisionInfo) {
        SDL_SetRenderDrawColor(canvas.renderer, info.color.r, info.color.g, info.color.b, 0xff);
        SDL_RenderDrawLineF(canvas.renderer, info.normalStart.x, info.normalStart.y, info.normalEnd.x, info.normalEnd.y);
    }

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
            if(canvas.evt.type == SDL_QUIT) shouldClose = true;
            processEvent(canvas);
        }

        SDL_SetRenderDrawColor(canvas.renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderClear(canvas.renderer);
        render(canvas);

        auto now = std::chrono::high_resolution_clock::now();
        auto dt = (now - lastTime).count() * 10e-9;
        lastTime = now;

        if(shouldRun) update(dt, canvas);

        SDL_RenderPresent(canvas.renderer);
    }
    
}


void processEvent(Canvas& cnv)
{
    if(cnv.evt.type == SDL_KEYDOWN) {
        switch (cnv.evt.key.keysym.sym)
        {
        case SDLK_w:
            bodies[1].angVel = 0.2f;
            break;
        
        default:
            break;
        }
    }

    if(cnv.evt.type == SDL_MOUSEMOTION) {
        float ex = cnv.evt.motion.x;
        float ey = cnv.evt.motion.y;
        auto& b1 = bodies[1];
        b1.pos.x = ex;
        b1.pos.y = ey;
    }
}


bool initCanvas(Canvas& canvas, int w, int h) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL failed to initialize" << std::endl;
        return false;
    }

    canvas.window = SDL_CreateWindow("RigidBody", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
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
    processEvent(cnv);
    mainLoop(cnv);
    return 0;
}