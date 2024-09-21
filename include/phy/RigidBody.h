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
            unsigned int color = 0xff0000;
            float mass = 1.0f;
            float im = 1.0f;    // moment of inertia
            float angVel = 0.0f;    // angular velocity
            
            bool isFilled = true;   
            Vector2 pos;
            Vector2 vel;
            vertices_t vertices;

            RigidBody() = default;

            explicit RigidBody(const vertices_t& v);
            void setRotation(float angle);
            float getRotation() const;

        private:
            float rotation = 0.0f;
    };


    RigidBody::RigidBody(const vertices_t& v)
    {
        vertices.clear();
        vertices.insert(vertices.end(), v.begin(), v.end());
    }

    inline void RigidBody::setRotation(float angle)
    {
        for(int i = 0; i < vertices.size(); i++) {
            vertices[i] = vertices[i].rotate(angle);
        }
        rotation = angle;
    }

    inline float RigidBody::getRotation() const
    {
        return rotation;
    }

} // namespace phy


#endif 