 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef MESH_HPP_INCLUDED
#define MESH_HPP_INCLUDED

#include "pre_ogre.hpp"

#include "types.hpp"

struct NewtonMesh;
struct NewtonCollision;

namespace engine
{

    namespace phys
    {

        class world;
        class mesh {
            private:
                NewtonMesh *_mesh;

            public:
                mesh(const string &mn, const mat4 &tf,const world &_world);
                mesh(const NewtonCollision *col);
                ~mesh();

                operator NewtonMesh *()
                {
                    return _mesh;
                }

                operator const NewtonMesh *() const
                {
                    return _mesh;
                }

//                NewtonCollision *convex(const world &w) const;
//                NewtonCollision *concave(const world &w) const;
//
//                NewtonMesh *m() const { return _mesh; }
//
//                static NewtonCollision *convexFromMesh(const world &w,
//                    const std::string &mn, const mat4 &tf = mat4::IDENTITY);
//
//                static NewtonCollision *concaveFromMesh(const world &w,
//                    const std::string &mn, const mat4 &tf = mat4::IDENTITY);
//
//                static NewtonCollision *treeFromMesh(const world &w,
//                    const std::string &mn, const mat4 &tf = mat4::IDENTITY,
//                    NewtonCollision *outsideTree = NULL, int outsideId = 0);
//
//                static NewtonCollision *box(const world &w, const vec3 &s);
//                static NewtonCollision *ball(const world &w, const vec3 &s);
//                static NewtonCollision *cylinder(const world &w, const vec3 &s);
//                static NewtonCollision *cone(const world &w, const vec3 &s);
//                static NewtonCollision *capsule(const world &w, const vec3 &s);
        };

    }

}

#endif // MESH_HPP_INCLUDED
