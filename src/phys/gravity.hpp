#ifndef GRAVITY_HPP_INCLUDED
#define GRAVITY_HPP_INCLUDED

#include "types.hpp"

namespace engine
{
    namespace phys
    {
        struct gravity_t
        {
            vec3 v;
            float strength;
            bool isPoint;
            bool enabled;

            gravity_t():
                v(0, -9.81, 0),
                strength(1),
                isPoint(false),
                enabled(false)
            {
            }
        };
    }
}

#endif // GRAVITY_HPP_INCLUDED
