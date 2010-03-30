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

#ifndef GUI_DISP_HPP_INCLUDED
#define GUI_DISP_HPP_INCLUDED

#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include "sheet.hpp"

namespace engine
{
    namespace gui
    {
        class guiDisp: public OIS::KeyListener, public OIS::MouseListener
        {
            protected:
				// TODO: comment this
                sheetPtr _dispatchSheet;

            public:
                /**
                 * Constructor
                 */
                guiDisp() {}

                /**
                 * Destructor
                 */
                virtual ~guiDisp() {}

                /**
                 * Event key pressed
                 */
                virtual bool keyPressed(const OIS::KeyEvent &e);
                /**
                 * Even key released
                 */
                virtual bool keyReleased(const OIS::KeyEvent &e);
                /**
                 * Event mouse moved
                 */
                virtual bool mouseMoved(const OIS::MouseEvent &e);
                /**
                 * Event mouse pressed
                 */
                virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
                /**
                 * Event mouse released
                 */
                virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
        };
    }
}

#endif // GUI_DISP_HPP_INCLUDED
