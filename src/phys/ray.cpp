 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <list>
#include <algorithm>

#include <Newton.h>

#include "world.hpp"
#include "body.hpp"
#include "ray.hpp"

namespace engine
{


    namespace phys
    {


        ray::ray():
            _p0(0, 0, 0),
            _p1(0, 0, 1)
        {
        }

        ray::~ray()
        {
        }

        bool ray::preHit(const body *b)
        {
            if (!b->active())
                return false;

            switch(_sm)
            {
                case SM_IGNORE:
                    return std::find(
                        specifics.begin(), specifics.end(), b) == specifics.end();
                break;

                case SM_ONLY:
                    return std::find(
                        specifics.begin(), specifics.end(), b) != specifics.end();
                break;

                case SM_ALL:
                    return true;
                break;
            }

            return true;
        }

        void ray::hit(body *b, const vec3 &n, float d, int id)
        {
            // interpolate distance [0..1] between points
            vec3 p = _p0 + (_p1 - _p0) * d;
            _hitCache.push_back(hitInfo(b, p, n, d));
        }

        unsigned preHitCB(
            const NewtonBody *_body, const NewtonCollision *_collision, void *userData)
        {
            return ((ray*)userData)->preHit(bodyCast(_body));
        }

        float hitCB(
            const NewtonBody *_body, const float *n, int id, void *userData, float d)
        {
            ((ray*)userData)->hit(bodyCast(_body), vec3(n[0], n[1], n[2]), d, id);
            return 1.0; // return 1 so search continues
        }

        struct hitInfoLess
        {
            bool operator()(const ray::hitInfo &lhs, const ray::hitInfo &rhs)
            {
                return lhs.d < rhs.d;
            }
        };

        const ray::hitList &ray::operator()(const world &w,
            const vec3 &p0, const vec3 &p1, ray::specificsMode sm)
        {
            _p0 = p0;
            _p1 = p1;
            _sm = sm;

            _hitCache.clear();

            NewtonWorldRayCast(w, &_p0.x, &_p1.x, &hitCB, this, &preHitCB);

            specifics.clear();

            // sort based on distance
            std::sort(_hitCache.begin(), _hitCache.end(), hitInfoLess());
            return _hitCache;
        }

    }


}

