 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef TIMER_HPP_INCLUDED
#define TIMER_HPP_INCLUDED

#include "pre_ogre.hpp"
#include "types.hpp"

namespace engine
{

    namespace time
    {

        class timer
        {
            private:

                Ogre::Timer *_timer;
                unsigned long _realTotal;
                real _total, _delta;

            public:

                real speed;

                real total() const { return _total; }
                real delta() const { return _delta; }

                void reset() { _total = 0; } // reset "fake" total

                real tick();

                real operator()() // some shorthand
                {
                    return tick();
                }

                timer(real speed = 1);
                ~timer();

        };

    }

}


#endif // TIMER_HPP_INCLUDED
