 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef INTERACTER_HPP_INCLUDED
#define INTERACTER_HPP_INCLUDED

#include "body.hpp"

namespace engine
{
    namespace phys
    {

        class world;

        class interacter
        {

            private:

                void _hook(body &b);
                boost::signals::connection _hookConnection;

                body *_body;

                // local hit position of body (in local space)
                vec3 _localPos;
                // position to hold from, direction to reach out to
                vec3 _pos, _dir;
                // distance from pos to the body's hit position in global space
                float _dist;

            public:

                interacter();
                ~interacter();

                void enable(const world &w);
                void disable();

                // update the positional info
                void operator()(const vec3 &p, const vec3 &d)
                {
                    _pos = p;
                    _dir = d;
                }
        };

    }
}

#endif // INTERACTER_HPP_INCLUDED
