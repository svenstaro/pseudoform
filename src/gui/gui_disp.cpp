 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "gui_disp.hpp"
#include "types.hpp"
namespace engine
{
    namespace gui
    {

        void guiDisp::initGUI(Ogre::RenderWindow *renderWindow, Ogre::SceneManager *sceneMgr)
        {
            mPlatform = new MyGUI::OgrePlatform();
            // Don't forget to send "data", it's the name of resource stream
            // to load in. Default is 'General'.
            mPlatform->initialise(renderWindow, sceneMgr);
            mPlatform->getRenderManagerPtr()->setActiveViewport(1);

            mGUI = new MyGUI::Gui();
            mGUI->initialise();
        }

        guiDisp::~guiDisp()
        {
            /*
            mGUI->shutdown();
            delete mGUI;
            mGUI = 0;

            mPlatform->shutdown();
            delete mPlatform;
            mPlatform = 0;*/
        }

        bool guiDisp::keyPressed(const OIS::KeyEvent &e)
        {
             return mGUI->injectKeyPress(MyGUI::KeyCode::Enum(e.key), e.text);
        }

        bool guiDisp::keyReleased(const OIS::KeyEvent &e)
        {
             return mGUI->injectKeyRelease(MyGUI::KeyCode::Enum(e.key));
        }

        bool guiDisp::mouseMoved(const OIS::MouseEvent &e)
        {
             return mGUI->injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);
        }

        bool guiDisp::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
        {
             return mGUI->injectMousePress(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
        }

        bool guiDisp::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
        {
             return mGUI->injectMouseRelease(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
        }
    }
}
