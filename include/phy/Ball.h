#ifndef __BYTENOL_PCGA_BALL_H__
#define __BYTENOL_PCGA_BALL_H__

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

        void render(SDL_Renderer* renderer);

        private: 
            void setColor(SDL_Renderer* renderer);
    };

    // Adapted from: https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl
    void Ball::render(SDL_Renderer* r) {
        float pi  = 3.14159265358979323846264338327950288419716939937510;
        float pih = pi / 2.0; //half of pi

        //drew  28 lines with   4x4  circle with precision of 150 0ms
        //drew 132 lines with  25x14 circle with precision of 150 0ms
        //drew 152 lines with 100x50 circle with precision of 150 3ms
        const int prec = 27; // precision value; value of 1 will draw a diamond, 27 makes pretty smooth circles.
        float theta = 0;     // angle that will be increased each loop

        //starting point
        int x0 = pos.x;
        int y0 = pos.y;
        int x  = (float)radius * cos(theta);//start point
        int y  = (float)radius * sin(theta);//start point
        int x1 = x;
        int y1 = y;

        setColor(r);

        //repeat until theta >= 90;
        float step = pih/(float)prec; // amount to add to theta each time (degrees)
        for(theta=step;  theta <= pih;  theta+=step)//step through only a 90 arc (1 quadrant)
        {
            //get new point location
            x1 = (float)radius * cosf(theta) + 0.5; //new point (+.5 is a quick rounding method)
            y1 = (float)radius * sinf(theta) + 0.5; //new point (+.5 is a quick rounding method)

            //draw line from previous point to new point, ONLY if point incremented
            if( (x != x1) || (y != y1) )//only draw if coordinate changed
            {
                SDL_RenderDrawLine(r, x0 + x, y0 - y,    x0 + x1, y0 - y1 );//quadrant TR
                SDL_RenderDrawLine(r, x0 - x, y0 - y,    x0 - x1, y0 - y1 );//quadrant TL
                SDL_RenderDrawLine(r, x0 - x, y0 + y,    x0 - x1, y0 + y1 );//quadrant BL
                SDL_RenderDrawLine(r, x0 + x, y0 + y,    x0 + x1, y0 + y1 );//quadrant BR
            }
            //save previous points
            x = x1;//save new previous point
            y = y1;//save new previous point
        }
        //arc did not finish because of rounding, so finish the arc
        if(x!=0)
        {
            x=0;
            SDL_RenderDrawLine(r, x0 + x, y0 - y,    x0 + x1, y0 - y1 );//quadrant TR
            SDL_RenderDrawLine(r, x0 - x, y0 - y,    x0 - x1, y0 - y1 );//quadrant TL
            SDL_RenderDrawLine(r, x0 - x, y0 + y,    x0 - x1, y0 + y1 );//quadrant BL
            SDL_RenderDrawLine(r, x0 + x, y0 + y,    x0 + x1, y0 + y1 );//quadrant BR
        }
    }

    inline void Ball::setColor(SDL_Renderer *renderer)
    {
        int r = (color >> 16) & 255;
        int g = (color >> 8) & 255;
        int b = color & 255;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    }
}

#endif 