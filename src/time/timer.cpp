 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgreTimer.h>
#include <OgreRoot.h>

#include "timer.hpp"

namespace engine
{

    namespace time
    {

        timer::timer(real speed):
            _timer(NULL),
            _realTotal(0),
            _total(0),
            _delta(0),
            speed(speed)
        {
            _timer = Ogre::Root::getSingleton().getTimer();
            _realTotal = _timer->getMilliseconds();
        }

        timer::~timer()
        {
        }

        real timer::tick()
        {
            unsigned long d = _timer->getMilliseconds() - _realTotal;
            _realTotal += d;

            _delta = real(d) * 0.001 * speed; // 0.001 = ms -> s
            _total += _delta;

            return _delta;
        }
    }

}
