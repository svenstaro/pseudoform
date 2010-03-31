 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

/**
 * @namespace engine
 * This namespace consist of functions related to the game-engine
 */
namespace engine
{
	/// Matrix 3
	typedef Ogre::Matrix3 mat3;
	/// Matrix 4
	typedef Ogre::Matrix4 mat4;

	/// Vector 2
	typedef Ogre::Vector2 vec2;
	/// Vector 3
	typedef Ogre::Vector3 vec3;
	/// Vector 4
	typedef Ogre::Vector4 vec4;

	/// Quaternion, this represents rotation in 3d-graphic
	typedef Ogre::Quaternion quat;
	/// Degrees
	typedef Ogre::Degree deg;
	/// Radians - standard for the measuring angles
	typedef Ogre::Radian rad;

	/// 3d-plane definition
	typedef Ogre::Plane plane;
	/// Real type
	typedef double real;

	/// Definition of material for using in rendering passes
	typedef std::pair<string, size_t> materialPassPair;
	/// Aligned box to the entity
	typedef Ogre::AxisAlignedBox aabb_t;
	/// Color value
	typedef Ogre::ColourValue colour;
	/// Game object interface type
	typedef unsigned interfaceType;
	/// List of interfaces
	typedef std::vector<interfaceType> interfaceList;

	/// Application argument options
	typedef std::vector<string> programOptions;

	/**
	 * @brief Convert array of float to matrix4
	 * @param m1 pointer to the array of float
	 * @return generated matrix4
	 */
    inline mat4 _matrix(const float *m1)
    {
        mat4 m0(mat4::IDENTITY);
        for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
                m0[i][j] = m1[i + j * 4];
        return m0;
    }

    /**
     * @brief Convert matrix4 to gived pointer of float array
     * @param m0  matrix to convert
     * @param m1  pointer to the float array
     * @return float array generated from matrix
     */
    inline float *_matrix(const mat4 &m0, float *m1)
    {
        for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
                m1[i + j * 4] = m0[i][j];
        return m1;
    }

    // TODO: Comment CLIP_SPACE_TO_IMAGE_SPACE
    static const mat4 CLIP_SPACE_TO_IMAGE_SPACE(
        0.5,    0,    0,  0.5,
        0,   -0.5,    0,  0.5,
        0,      0,    1,    0,
        0,      0,    0,    1);
}

/**
 * @brief Overriden binary <= operator  for vector2 type
 * @param a  vector2, first operand
 * @param b  vector2, second operand
 *
 * @return true if a <= b
 */
inline bool operator <=(const engine::vec2 &a, const engine::vec2 &b)
{
    return a < b || a == b;
}

/**
 * @brief Overriden binary >= operator  for vector2 type
 * @param a  vector2, first operand
 * @param b  vector2, second operand
 *
 * @return true if a >= b
 */
inline bool operator >=(const engine::vec2 &a, const engine::vec2 &b)
{
    return a > b || a == b;
}

#endif // TYPES_HPP_INCLUDED
