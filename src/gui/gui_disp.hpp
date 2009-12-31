 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
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

                sheetPtr _dispatchSheet;

            public:

                guiDisp() {}
                virtual ~guiDisp() {}

                virtual bool keyPressed(const OIS::KeyEvent &e);
                virtual bool keyReleased(const OIS::KeyEvent &e);
                virtual bool mouseMoved(const OIS::MouseEvent &e);
                virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
                virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
        };

    }

}

#endif // GUI_DISP_HPP_INCLUDED
