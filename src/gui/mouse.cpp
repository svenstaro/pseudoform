 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgrePass.h>
#include <OgreViewport.h>

#include "mouse.hpp"

namespace engine
{
    namespace gui
    {

        mouse::mouse(const string &name):
            panel(name)
        {
            _interfaces.push_back(WIDGET_MOUSE);
            textureName("gui/mouse.png");
            fitTextureSize();
        }

        mouse::~mouse()
        {
        }

    }

}
