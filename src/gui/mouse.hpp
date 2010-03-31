 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

/*
 * Update list:
 * date
 * comments
 */

#ifndef MOUSE_HPP_INCLUDED
#define MOUSE_HPP_INCLUDED

#include "panel.hpp"

namespace engine
{
    namespace gui
    {
		/**
		 * This class represents mouse widget (cursor)
		 */
        class mouse: public panel
        {
            public:
				/**
				 * Constructor
				 */
                mouse(const string &name);

                /**
                 * Destructor
                 */
                ~mouse();

                // TODO: Remove if not needed
                //void _renderThis(Ogre::Viewport *vp);
        };
    }
}

#endif // MOUSE_HPP_INCLUDED
