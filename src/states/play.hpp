 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PLAY_HPP_INCLUDED
#define PLAY_HPP_INCLUDED

#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include "time/timer.hpp"
#include "gfx/renderer.hpp"

#include "game/world.hpp"
#include "game/entity.hpp"

#include "phys/world.hpp"

#include "script/script.hpp"

#include "game/level.hpp"

#include "state.hpp"

#include "gui/gui_disp.hpp"

#include "action_binder.hpp"

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

    class playState : public state, public engine::gui::guiDisp, public engine::actionBinder
    {
        private:

            const engine::input::input &_input;
            engine::snd::system &_soundSys;
            engine::gui::sheetPtr _sheet;
            // graphics-related
            Ogre::SceneManager *_sceneMgr;
            Ogre::Viewport *_viewport;
            engine::gfx::rendererPtr _renderer;
            // timer
            engine::time::timer _timer;

            levelPtr _level;

            bool _shouldExit;

            void _playerControl();

            void _createGUI();
            void _destroyGUI();
            void _createScript();
            void _destroyScript();

        public:

            void loadLevel(const engine::string &fn);
            void jump();
            void move(const engine::vec3 &v);
            void clear();
            void exit() { _shouldExit = true; }

            playState(
                const engine::input::input &input,
                engine::snd::system &soundSys,
                Ogre::SceneManager *mgr, Ogre::Viewport *vp,
                engine::programOptions *opts = NULL);

            // init from another state
            playState(
                const engine::input::input &input,
                engine::snd::system &soundSys,
                const engine::gfx::rendererPtr &renderer,
                const levelPtr &lvl);

            ~playState();

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

#endif // PLAY_HPP_INCLUDED
