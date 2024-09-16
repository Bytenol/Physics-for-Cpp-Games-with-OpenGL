#ifndef __BYTENOL_PCGA_BALL_H__
#define __BYTENOL_PCGA_BALL_H__

#include <cmath>
#include <SDL.h>
#include "Vector.h"

namespace phy {

    struct Ball
    {
        int radius = 20;
        float mass = 1.0f;
        unsigned int color = 0xff0000;
        
        Vector2 pos;
        Vector2 vel;

        static bool isStroked;

        void render(SDL_Renderer* renderer);

        private: 
            void setColor(SDL_Renderer* renderer);
            void drawFilled(SDL_Renderer* renderer);
            void drawStroked(SDL_Renderer* renderer);
    };

    // Adapted from: https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl
    void Ball::render(SDL_Renderer* r) {
        setColor(r);
        if(isStroked) drawStroked(r);
        else drawFilled(r);
    }

    inline void Ball::setColor(SDL_Renderer *renderer)
    {
        int r = (color >> 16) & 255;
        int g = (color >> 8) & 255;
        int b = color & 255;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    }



    inline void Ball::drawFilled(SDL_Renderer *r)
    {
        auto drawHorizontalLine = [](SDL_Renderer* renderer, int x1, int x2, int y) -> void {
            for (int x = x1; x <= x2; x++)
                SDL_RenderDrawPoint(renderer, x, y);
        };
        
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;

        while (y >= x) {
            // Draw horizontal lines (scanlines) for each section of the circle
            drawHorizontalLine(r, pos.x - x, pos.x + x, pos.y - y);
            drawHorizontalLine(r, pos.x - x, pos.x + x, pos.y + y);
            drawHorizontalLine(r, pos.x - y, pos.x + y, pos.y - x);
            drawHorizontalLine(r, pos.x - y, pos.x + y, pos.y + x);

            // Update decision parameter and points
            if (d < 0) {
                d = d + 4 * x + 6;
            } else {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
            
        }
    }


    inline void Ball::drawStroked(SDL_Renderer *renderer)
    {
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;

        while (y >= x) {
            // Draw the 8 symmetrical points of the circle
            SDL_RenderDrawPoint(renderer, pos.x + x, pos.y + y);
            SDL_RenderDrawPoint(renderer, pos.x - x, pos.y + y);
            SDL_RenderDrawPoint(renderer, pos.x + x, pos.y - y);
            SDL_RenderDrawPoint(renderer, pos.x - x, pos.y - y);
            SDL_RenderDrawPoint(renderer, pos.x + y, pos.y + x);
            SDL_RenderDrawPoint(renderer, pos.x - y, pos.y + x);
            SDL_RenderDrawPoint(renderer, pos.x + y, pos.y - x);
            SDL_RenderDrawPoint(renderer, pos.x - y, pos.y - x);

            // Update the decision parameter and points
            if (d < 0) {
                d = d + 4 * x + 6;
            } else {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }
    }


    bool Ball::isStroked = true;
}

#endif 