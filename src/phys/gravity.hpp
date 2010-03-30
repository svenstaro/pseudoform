 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef GRAVITY_HPP_INCLUDED
#define GRAVITY_HPP_INCLUDED

#include "types.hpp"

namespace engine
{
    namespace phys
    {
		/// Represents gravity force
        struct gravity_t
        {
        	/// Gravity vector
            vec3 v;
            /// Gravity strengh
            float strength;
            // TODO: gravity isPoint
            bool isPoint;
            /// Use gravity?
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
