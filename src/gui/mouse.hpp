 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef MOUSE_HPP_INCLUDED
#define MOUSE_HPP_INCLUDED

#include "panel.hpp"

namespace engine
{
    namespace gui
    {

        class mouse: public panel
        {
            public:

                mouse(const string &name);
                ~mouse();

                //void _renderThis(Ogre::Viewport *vp);

        };
    }
}

#endif // MOUSE_HPP_INCLUDED
