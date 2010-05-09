 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef MAIN_MENU_HPP_INCLUDED
#define MAIN_MENU_HPP_INCLUDED

#include <list>

#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <Ogre.h>

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

/**
 * @namespace game
 * @brief Consist of tools for building game world
 */
namespace game
{
	/**
	 * @class game
	 * @brief Menu game state
	 *
	 * Implements game menu state
	 */
    class mainMenuState : public state, public engine::gui::guiDisp
    {
        private:
			/// Input manager handler
            const engine::input::input &_input;

            /// Sound system handler
            engine::snd::system &_soundSys;

            /// Ogre's scene manager to use in this state
            Ogre::SceneManager *_sceneMgr;

            /// Viewport to use in this state
            Ogre::Viewport *_viewport;

            /// Render window
            Ogre::RenderWindow *_window;

            /// Program arguments options
            engine::programOptions *_opts;

			/// Timer object for the render cycle
            engine::time::timer _realTimer;

            /**
             * @brief Constructor
             * @remarks Doesn't implement copying
             */
            mainMenuState(const mainMenuState &rhs);

            /**
             * @brief Create gui sheet for current state
             */
            void _createGUI();

            /**
             * @brief Remove gui
             */
            void _destroyGUI();

            /**
             * @enum whatToRun
             * @brief Available menu items
             */
            enum whatToRun
            {
                RUN_PLAY, RUN_EDITOR, RUN_CREDITS, RUN_NOTHING
            } _whatToRun;

            /**
             * @brief Calls chosen from menu state to run
             * @param r available menu item
             */
            void _clickButton(whatToRun r);

        public:
            /**
             * Constructor
             * @param input input manager handler
             * @param soundSys sound system handler
             * @param mgr scene manager handler
             * @param vp viewport
             * @param opts argument parameters
             */
            mainMenuState(
                const engine::input::input &input,
                engine::snd::system &soundSys,
                Ogre::SceneManager *mgr, Ogre::Viewport *vp, Ogre::RenderWindow *rWindow,
                engine::programOptions *opts = NULL);

            /**
             * Destructor
             */
            ~mainMenuState();

            // This is commented in state class

            void tick(stateManager &mgr);
            void render();

            void suspend();
            void resume();

            // We don't need to overload this method due
            // they are already implemented in gui_disp.cpp
            /**
             * @brief Event key pressed
             */
            //virtual bool keyPressed(const OIS::KeyEvent &e);
            /**
             * @brief Even key released
             */
            //virtual bool keyReleased(const OIS::KeyEvent &e);
            /**
             * @brief Event mouse moved
             */
            //virtual bool mouseMoved(const OIS::MouseEvent &e);
            /**
             * @brief Event mouse pressed
             */
            //virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
            /**
             * @brief Event mouse released
             */
            //virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    };
}

#endif // MAIN_MENU_HPP_INCLUDED
