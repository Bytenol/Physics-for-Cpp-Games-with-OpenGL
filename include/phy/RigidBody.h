#ifndef __BYTENOL_PCGA_RIGID_BODY_H__
#define __BYTENOL_PCGA_RIGID_BODY_H__

#include <vector>
#include <cmath>
#include "Vector.h"

namespace phy
{
    using vertices_t = std::vector<Vector2>;
    
    /**
     * Base class for rigid bodies
     */
    class RigidBody
    {
        public:
            float mass = 1.0f;
            float im = 5000.0f;    // moment of inertia
            float angVel = 0.0f;    // angular velocity
            float rotation = 0.0f;
            
            Vector2 pos;
            Vector2 vel;
            vertices_t vertices;

            struct color { 
                unsigned short r = 255;
                unsigned short g = 0;
                unsigned short b = 0;
            } color;

            RigidBody() = default;
            explicit RigidBody(const vertices_t& v);
    };


    RigidBody::RigidBody(const vertices_t& v)
    {
        vertices.clear();
        vertices.insert(vertices.end(), v.begin(), v.end());
    }

} // namespace phy


#endif 