/**
 * @file SAT_resolution.cpp
 * @brief a simple implementation of the seperating axis theorem
 * @date 29th Sept, 2024
 * 
 * This file shows the implementation of the seperating axis theorem, A 
 * common collision detection techniques that works for any convex polygons
 * and quadrilaterals and how to stop them from penetration
 * 
 * @todo does not fully solve the penetrations
 */

#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include <cmath>
#include <random>

#include <SDL.h>
#include <phy/Vector.h>

using namespace phy;

struct Canvas {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event evt;
    int w;
    int h;
};


struct Polygon
{
    struct {
        unsigned int r = 255;
        unsigned int g = 0;
        unsigned int b = 255;
    } color;

    Vector2 pos;
    Vector2 vel;
    float rotation = 0.0f;
    float angVel = 0.0f;
    float radius = 0.0f;

    // original vertex data without any transformation
    std::vector<Vector2> vertices;  

    // transformed vertex data: always get updated in every frame
    std::vector<Vector2> transformed;   

    Polygon() = default;
    Polygon(const std::vector<Vector2>& vertices)
    {
        this->vertices = vertices;
        this->transformed = vertices;
    }
};


struct CollisionInfo
{
    float projectionLength = 0.0f;
    float collisionDepth = 0.0f;
    Vector2 normal;
};


void makePolygon(int amount, const Canvas& canvas);

/// @brief Carry out seperating axis theorem algorithms on polygons
/// @param polygon The polygon to check 
/// @param polygon2 The potential polygon it will collide with
/// @return true if there is any collision
bool sat_collision(Polygon& polygon, Polygon& polygon2);

const float g = 10.0f;
std::vector<Polygon> polygons;


void init(const Canvas& cnv) {
    makePolygon(25, cnv);
    polygons[1].vel = Vector2(0, 0); 
        Polygon polygon = Polygon{
        {
            { 0.0f, -50.0f },
            { 50.0f, 50.0f },
            { -50.0f, 50.0f }
        }
    };
    polygon.pos.x = 300;
    polygon.pos.y = 300;
    // polygon.vel.x = -34;
    polygon.angVel = 0.0f;
    polygons.push_back(polygon);

    polygon = Polygon{
        {
            {-50.0f, -25.0f},
            { 50.0f, -25.0f },
            { 50.0f, 25.0f },
            { -50.0f, 25.0f }
        }
    };
    polygon.pos.x = 150;
    polygon.pos.y = 200;
    // polygon.vel.x = 50.0f;
    polygon.angVel = 0.0f;
    // polygon.rotation = (45 * 3.14159 / 180);
    polygons.push_back(polygon);
}


void update(float dt, Canvas& canvas) 
{
// transform the whole polygon
    for(auto& polygon: polygons)
    {
        polygon.pos += polygon.vel * dt;
        polygon.rotation += polygon.angVel * dt;
        for(int i = 0; i < polygon.vertices.size(); i++)
            polygon.transformed[i] = polygon.pos + polygon.vertices[i].rotate(polygon.rotation);
    }

    for(auto polygon = polygons.begin(); polygon != polygons.end(); polygon++)
    {
        polygon->color.b = 255;

        // wall boundary check
        if(polygon->pos.x - polygon->radius <= 0) {
            polygon->pos.x = polygon->radius;
            polygon->vel.x *= -1;
        } else if(polygon->pos.x + polygon->radius >= canvas.w) {
            polygon->pos.x = canvas.w - polygon->radius;
            polygon->vel.x *= -1;
        }

        if(polygon->pos.y - polygon->radius <= 0) {
            polygon->pos.y = polygon->radius;
            polygon->vel.y *= -1;
        } else if(polygon->pos.y + polygon->radius >= canvas.h) {
            polygon->pos.y = canvas.h - polygon->radius;
            polygon->vel.y *= -1;
        }

        // check for sat collision
        for(auto polygon2 = polygon + 1; polygon2 != polygons.end(); polygon2++)
            if(sat_collision(*polygon, *polygon2))
                polygon->color.b = 0;
    }

}


void render(Canvas& canvas) 
{
    // draw polygons
    for(auto& body: polygons)
    {
        SDL_SetRenderDrawColor(canvas.renderer, body.color.r, body.color.g, body.color.b, 255);
        for(size_t i = 0; i < body.vertices.size(); i++)
        {
            auto v1 = body.transformed[i];
            auto v2 = body.transformed[(i + 1) % body.transformed.size()];
            SDL_RenderDrawLine(canvas.renderer, v1.x, v1.y, v2.x, v2.y);
        }
    }
}


bool sat_collision(Polygon& polygon, Polygon& polygon2)
{
    std::vector<CollisionInfo> collisionInfo;

    Polygon* poly1 = &polygon;
    Polygon* poly2 = &polygon2;
    float overlap = INFINITY;
    
    for(int i = 0; i < 2; i++)
    {
        if(i > 0)
        {
            poly1 = &polygon2;
            poly2 = &polygon;
        }

        for(int i = 0; i < poly1->transformed.size(); i++)
        {
            int inext = (i + 1) % poly1->transformed.size();
            auto p1 = poly1->transformed[i];
            auto p2 = poly1->transformed[inext];
            auto vDir = p2 - p1;
            auto normal = Vector2{ vDir.y, -vDir.x }.normalize();

            float min_1 = INFINITY, max_1 = -INFINITY;
            for(auto it = poly1->transformed.begin(); it != poly1->transformed.end(); it++)
            {
                float dp = it->dotProduct(normal);
                min_1 = std::min(min_1, dp);
                max_1 = std::max(max_1, dp);
            }

            float min_2 = INFINITY, max_2 = -INFINITY;
            for(auto it = poly2->transformed.begin(); it != poly2->transformed.end(); it++)
            {
                float dp = it->dotProduct(normal);
                min_2 = std::min(min_2, dp);
                max_2 = std::max(max_2, dp);
            }

            overlap = std::min(std::min(max_1, max_2) - std::max(min_1, min_2), overlap);

            if(!(min_1 <= max_2 && min_2 <= max_1))
                return false;
        }
    }

    auto g = (poly1->pos - poly2->pos).normalize() * overlap;
    poly1->pos += g;

    return false;
}


void handleEvent(SDL_Event* evt)
{
    if(evt->type == SDL_MOUSEMOTION)
    {
        polygons[1].pos.x = evt->motion.x;
        polygons[1].pos.y = evt->motion.y;
    }
    
    if(evt->type == SDL_KEYDOWN) {
        if(evt->key.keysym.sym == 4) {
            polygons[1].rotation += 1.f;
        }
    }
}


void makePolygon(int amount, const Canvas& canvas)
{
    std::random_device rd;
    std::mt19937 eng(rd());

    auto randRange = [&eng](int min, int max) {
        std::uniform_int_distribution<> distr(min, max);
        return distr(eng);
    };

    for(int i=0; i < amount; i++)
    {
        int sides = randRange(3, 6);
        int step = 360 / sides;

        float radius = randRange(5, 50);

        // polar coordinates
        std::vector<Vector2> vertices;
        for(int j = 0; j < 360; j += step)
        {
            float angle = j * 3.14159f / 180;
            vertices.push_back({ std::cos(angle) * radius, std::sin(angle) * radius });
        }

        Polygon polygon{ vertices };
        polygon.radius = radius;
        polygon.pos.x = randRange(radius * 1.4, canvas.w - radius * 1.4);
        polygon.pos.y = randRange(radius * 1.4, canvas.h - radius * 1.4);

        float vAng = randRange(0, 360) * 3.14159f / 180;
        polygon.vel = Vector2(std::cos(vAng) * radius, std::sin(vAng) * 3);
        polygon.angVel = randRange(-40, 40) * 3.14159 / 180;

        polygons.push_back(polygon);
    }
}



void mainLoop(Canvas& canvas) {
    bool shouldClose = false;

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!shouldClose)
    {
        while(SDL_PollEvent(&canvas.evt) != 0)
        {
            if(canvas.evt.type == SDL_QUIT) 
            shouldClose = true;
            handleEvent(&canvas.evt);
        }

        SDL_SetRenderDrawColor(canvas.renderer, 0x00, 0x00, 0x00, 0xff);
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

    canvas.window = SDL_CreateWindow("SAT Resolution", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
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
    mainLoop(cnv);
    return 0;
}