 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef TYPES_HPP_INCLUDED
#define TYPES_HPP_INCLUDED

#include <utility>

#include <vector>

#include <OgreMatrix4.h>
#include <OgreVector3.h>
#include <OgreVector4.h>
#include <OgreVector2.h>
#include <OgreQuaternion.h>
#include <OgreMath.h>
#include <OgreCommon.h>
#include <OgreColourValue.h>
#include <OgreAxisAlignedBox.h>

#include "string.hpp"

namespace engine
{

    // newton matrix is column-major
    // 00 04 08 12
    // 01 05 09 13
    // 02 06 10 14
    // 03 07 11 15

    // ogre matrix is row-major
    // (must transpose)

    typedef Ogre::Matrix4 mat4;
    typedef Ogre::Matrix3 mat3;
    typedef Ogre::Vector3 vec3;
    typedef Ogre::Vector4 vec4;
    typedef Ogre::Vector2 vec2;
    typedef Ogre::Quaternion quat;
    typedef double real;
    typedef Ogre::Degree deg;
    typedef Ogre::Radian rad;
    typedef Ogre::Plane plane;
    typedef std::pair<string, size_t> materialPassPair;
    typedef Ogre::AxisAlignedBox aabb_t;
    typedef Ogre::ColourValue colour;
    typedef unsigned interfaceType;
    typedef std::vector<interfaceType> interfaceList;

    typedef std::vector<string> programOptions;

//    inline vec3 project(const vec3 &v, const vec3 &n)
//    {
//        return plane(n, 0).projectVector(v);
//    }

    inline mat4 _matrix(const float *m1)
    {
        mat4 m0(mat4::IDENTITY);
        for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
                m0[i][j] = m1[i + j * 4];
        return m0;
    }

    inline float *_matrix(const mat4 &m0, float *m1)
    {
        for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
                m1[i + j * 4] = m0[i][j];
        return m1;
    }

    static const mat4 CLIP_SPACE_TO_IMAGE_SPACE(
        0.5,    0,    0,  0.5,
        0,   -0.5,    0,  0.5,
        0,      0,    1,    0,
        0,      0,    0,    1);

}

// vec2 no <= and >= operators?! wtf?

inline bool operator <=(const engine::vec2 &a, const engine::vec2 &b)
{
    return a < b || a == b;
}

inline bool operator >=(const engine::vec2 &a, const engine::vec2 &b)
{
    return a > b || a == b;
}



#endif // TYPES_HPP_INCLUDED
