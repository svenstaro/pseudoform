 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef GUI_DISP_HPP_INCLUDED
#define GUI_DISP_HPP_INCLUDED

#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include "MyGUI_OgreRenderManager.h"

namespace engine
{
    namespace gui
    {
        class guiDisp: public OIS::KeyListener, public OIS::MouseListener
        {
            protected:
                MyGUI::Gui *mGUI;
                MyGUI::OgrePlatform *mPlatform;

            public:
                /**
                * @brief Configure gui system
                */
                virtual void initGUI(Ogre::RenderWindow *renderWindow, Ogre::SceneManager *sceneMgr);

                /**
                 * Constructor
                 */
                guiDisp() {}

                /**
                 * Destructor
                 */
                virtual ~guiDisp();

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
