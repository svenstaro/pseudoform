 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef MOUSE_BUTTON_HPP_INCLUDED
#define MOUSE_BUTTON_HPP_INCLUDED

namespace engine
{
    namespace gui
    {
		/// List of available mouse buttons
        enum mouseButton
        {
            MB_LEFT,
            // what kind of fucking idiot used a #define for this?
            // ^ :D max
            #undef MB_RIGHT
            MB_RIGHT,
            MB_MIDDLE,
            MB_AUX0,
            MB_AUX1,
            MB_AUX2,
            MB_AUX3,
            MB_AUX4
        };
    }
}

#endif // MOUSE_BUTTON_HPP_INCLUDED
