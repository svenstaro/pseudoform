 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef RAY_HPP_INCLUDED
#define RAY_HPP_INCLUDED

#include <vector>

#include "types.hpp"

namespace engine
{

    namespace phys
    {

        class body;
        class world;

        class ray
        {

            private:

                //const world &_world;

            public:

                struct hitInfo
                {
                    body *b; // hit body
                    vec3 p, n; // position, normal (world space)
                    float d; // [0..1] distance between p0 and p1

                    hitInfo(body *b, const vec3 &p, const vec3 &n, float d):
                        b(b), p(p), n(n), d(d) {}
                };

                typedef std::vector<hitInfo> hitList;

                typedef std::vector<body*> specificsList;
                specificsList specifics;

                enum specificsMode
                {
                    SM_IGNORE, SM_ONLY, SM_ALL
                };

                ray();
                ~ray();

                const hitList &operator()(const world &w,
                    const vec3 &p0, const vec3 &p1, specificsMode sm = SM_ALL);

                bool preHit(const body *b);
                void hit(body *b, const vec3 &n, float d, int id);

            private:

                hitList _hitCache;

                vec3 _p0, _p1;
                specificsMode _sm;

        };

    }



}


#endif // RAY_HPP_INCLUDED
