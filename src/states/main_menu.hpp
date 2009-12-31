 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef MAIN_MENU_HPP_INCLUDED
#define MAIN_MENU_HPP_INCLUDED

//#include <map>
#include <list>

#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include "gui/gui_disp.hpp"
#include "time/timer.hpp"

#include "state.hpp"

#include "pre_ogre.hpp"
#include "types.hpp"

namespace engine
{
    namespace input
    {
        class input;
    }

    namespace snd
    {
        class system;
    }
}

namespace game
{

    class mainMenuState : public state, public engine::gui::guiDisp
    {
        private:

            const engine::input::input &_input;
            engine::snd::system &_soundSys;
            engine::gui::sheetPtr _sheet;
            // graphics-related
            Ogre::SceneManager *_sceneMgr;
            Ogre::Viewport *_viewport;
            engine::programOptions *_opts;

            // timer
            engine::time::timer _realTimer;

            mainMenuState(const mainMenuState &rhs); // no copying

            void _createGUI();
            void _destroyGUI();

            enum whatToRun
            {
                RUN_PLAY, RUN_EDITOR, RUN_CREDITS, RUN_NOTHING
            } _whatToRun;

            void _clickButton(whatToRun r);

        public:

            mainMenuState(
                const engine::input::input &input,
                engine::snd::system &soundSys,
                Ogre::SceneManager *mgr, Ogre::Viewport *vp,
                // possibly load a level at startup or something
                engine::programOptions *opts = NULL);

            ~mainMenuState();

            void tick(stateManager &mgr);
            void render();

            void suspend();
            void resume();

            virtual bool keyPressed(const OIS::KeyEvent &e);
            virtual bool keyReleased(const OIS::KeyEvent &e);
            virtual bool mouseMoved(const OIS::MouseEvent &e);
            virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
            virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    };

}

#endif // MAIN_MENU_HPP_INCLUDED
