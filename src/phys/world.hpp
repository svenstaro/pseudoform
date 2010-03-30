 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PHYS_WORLD_HPP_INCLUDED
#define PHYS_WORLD_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include "types.hpp"
#include "gravity.hpp"

struct NewtonWorld;
struct NewtonBody;

namespace engine
{
    namespace phys
    {
		/**
		 * Represents global physics world
		 */
        class world
        {
            private:
				/// Newtona main world-oject
                NewtonWorld *_world;

                // TODO: accum, freq
                real _accum;
                real _freq;

                /**
                 * Constructor
                 * @remarks doon't have copying constructor
                 */
                world(const world &rhs);

                // TODO: physic lastStep
                real _lastStep;

            public:

                real freq() const { return _freq; }
                void freq(real f);

                real lastStepSize() const { return _lastStep; }

                void clearCache() const;
                void threads(unsigned n);
                void platform(unsigned n);
                void friction(unsigned n);
                void solver(unsigned n);

                enum
                {
                    PLAT_DEFAULT = 0,
                    PLAT_OPTMIZED = 3,
                    FRIC_EXACT = 0,
                    FRIC_ADAPTIVE = 1,
                    SOLVER_EXACT = 0,
                    SOLVER_ADAPTIVE = 1
                };

                operator NewtonWorld *() const
                {
                    return _world;
                }

                /**
                 * Constructor
                 */
                world();
                /**
                 * Destructor
                 */
                ~world();

                /**
                 * Set physics tick
                 * @remarks dt CAN tick backwards (negative)
                 */
                void tick(real dt);

                /**
                 * Reset the accumulator
                 */
                void resetAccum() { _accum = 0; }

                // TODO: physic wake
                void wake(const aabb_t &aabb) const;

                /// Gravity handler
                gravity_t gravity;
        };

        typedef boost::shared_ptr<world> worldPtr;
    }
}

#endif // PHYS_WORLD_HPP_INCLUDED
