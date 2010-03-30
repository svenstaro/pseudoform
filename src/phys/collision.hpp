 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef COLLISION_HPP_INCLUDED
#define COLLISION_HPP_INCLUDED

#include <vector>

#include <boost/shared_ptr.hpp>

struct NewtonCollision;

#include "types.hpp"

namespace engine
{
    namespace phys
    {
        class world;

        class collision
        {
            private:

                // no-copy
                collision(const collision&);
                collision &operator=(const collision&);

            protected:

                const world &_world;

                NewtonCollision *_col;

                vec3 _scale;

            public:

                collision(const world &w): _world(w), _col(NULL), _scale(1, 1, 1) {}
                // assume no reference addition is needed
                explicit collision(const world &w, NewtonCollision *c):
                    _world(w),
                    _col(c),
                    _scale(1, 1, 1) {}
                virtual ~collision();

                operator NewtonCollision *()
                {
                    return _col;
                }

                operator const NewtonCollision *() const
                {
                    return _col;
                }

                const vec3 &scale() const { return _scale; }
                virtual void scale(const vec3 &v) = 0;
                virtual float volume() const = 0;

                virtual collision *clone() const = 0;
        };

        typedef boost::shared_ptr<collision> collisionPtr;

        collisionPtr box(const world &w, const vec3 &diameter);
        collisionPtr ball(const world &w, const vec3 &diameter);
        collisionPtr cone(const world &w, float diameter, float height);
        collisionPtr cylinder(const world &w, float diameter, float height);
        collisionPtr capsule(const world &w, float diameter, float height);
        collisionPtr null(const world &w);

        class convexHullCollision: public collision
        {

            protected:

                NewtonCollision *_orig;

                // construct from original
                convexHullCollision(const world &w, NewtonCollision *orig);

            public:

                convexHullCollision(const world &w, const string &meshName, const vec3 &s);
                ~convexHullCollision();

                void scale(const vec3 &v);
                float volume() const;

                convexHullCollision *clone() const;

        };

        class concaveHullCollision: public collision
        {

            protected:

                std::vector<NewtonCollision*> _origs;
                std::vector<NewtonCollision*> _parts;

                // copy-construct from original parts
                concaveHullCollision(const world &w, const std::vector<NewtonCollision*> &origs);

            public:

                concaveHullCollision(const world &w, const string &meshName, const vec3 &s);
                ~concaveHullCollision();

                void scale(const vec3 &v);
                float volume() const;

                concaveHullCollision *clone() const;

        };

        class treeCollision: public collision
        {

            protected:

                string _meshName;

                vec3 _origScale;
                void create(const vec3 &s);

            public:

                treeCollision(const world &w, const string &meshName, const vec3 &s);

                void scale(const vec3 &v);
                float volume() const;

                treeCollision *clone() const;

        };

        class nullCollision: public collision
        {

            public:

                nullCollision(const world &w);

                void scale(const vec3 &v) {} // no scaling null shapes
                float volume() const { return 1; } // just so it doesnt divide by 0

                nullCollision *clone() const
                {
                    return new nullCollision(_world);
                }

        };

        class primitiveCollision: public collision
        {

            protected:

                NewtonCollision *_orig;

            public:

                primitiveCollision(const world &w, NewtonCollision *c);
                ~primitiveCollision();

                void scale(const vec3 &v);
                float volume() const;

                primitiveCollision *clone() const;

        };
    }
}

#endif // COLLISION_HPP_INCLUDED
