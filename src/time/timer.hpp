 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef TIMER_HPP_INCLUDED
#define TIMER_HPP_INCLUDED

#include "pre_ogre.hpp"
#include "types.hpp"

/**
 * @namespace engine
 * @brief Consist of functions related to the game-engine
 */
namespace engine
{
	/**
	 * @namespace time
	 * @brief Represents interface for working with time.
	 */
    namespace time
    {
		/**
		 * @class timer
		 * @brief Base timer class
		 *
		 * We use this in global application cycle for updating:
		 *  - 3d objects
		 *  - physics
		 *  - GUI
		 */
        class timer
        {
            private:
				/// Base Ogre timer
                Ogre::Timer *_timer;

                /// Time when last frame rendered
                unsigned long _realTotal;

				/// Total time in game
                real _total;

                /// Difference in time between frames
                real _delta;

            public:
                /// Speed of time in global world
                real speed;

                /**
                 * @brief Get total time from timer
                 */
                real total() const { return _total; }

                /**
                 * @brief Get difference in time between frames
                 */
                real delta() const { return _delta; }

                /**
                 * @brief Reset total time in game
                 */
                void reset() { _total = 0; } // reset "fake" total

                /**
                 * @brief Game-time step(tick)
                 */
                real tick();

                real operator()()
                {
                    return tick();
                }

                /**
                 * Constructor
                 * @param speed game time speed. Default = 1(normal)
                 */
                timer(real speed = 1);

                /**
                 * Destructor
                 */
                ~timer();
        };
    }
}


#endif // TIMER_HPP_INCLUDED
