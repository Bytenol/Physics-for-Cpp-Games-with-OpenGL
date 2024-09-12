#ifndef __BYTENOL_PCGA_WALL_H__
#define __BYTENOL_PCGA_WALL_H__

#include <SDL.h>
#include "Vector.h"

namespace phy {

    struct Wall
    {
        Vector2 start;
        Vector2 end;
        
        void render(SDL_Renderer* renderer) const;
    };

    void Wall::render(SDL_Renderer *renderer) const
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderDrawLineF(renderer, start.x, start.y, end.x, end.y);
    }
}

#endif 