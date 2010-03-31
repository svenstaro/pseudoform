 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/bind.hpp>

#include "gui/mouse.hpp"
#include "gui/panel.hpp"
// TODO: implement window.hpp and scrollbar.hpp
//#include "gui/window.hpp"
#include "gui/gui_disp.hpp"
//#include "gui/scrollbar.hpp"
#include "gui/text.hpp"
#include "gui/button.hpp"
#include "gui/skin.hpp"
#include "gui/layout.hpp"

#include "input/input.hpp"
#include "input/keys.hpp"

#include "snd/system.hpp"

#include "main_menu.hpp"
// TODO: implement editor.hpp and play.hpp
//#include "editor.hpp"
//#include "play.hpp"
#include "data_dir.hpp"
#include "fs_ops.hpp"
#include "log.hpp"

#include <OgreSceneManager.h>
#include <OgreViewport.h>

namespace game
{
    mainMenuState::mainMenuState(
        const engine::input::input &input,
        engine::snd::system &soundSys,
        Ogre::SceneManager *mgr, Ogre::Viewport *vp,
        engine::programOptions *opts):
        _input(input),
        _soundSys(soundSys),
        _sceneMgr(mgr),
        _viewport(vp),
        _opts(opts),
        _whatToRun(RUN_NOTHING)
    {
        using namespace engine;

        _createGUI();

        // Route input to us
        _input(this, this);
    }

    mainMenuState::~mainMenuState()
    {
        _destroyGUI();
    }

    void mainMenuState::suspend()
    {
    }

    void mainMenuState::resume()
    {
        // Route input to us
        _input(this, this);
    }

    void mainMenuState::_createGUI()
    {
        using namespace engine;
        using namespace engine::gui;

        _sheet.reset(new sheet("mainMenuState::_sheet"));
        _dispatchSheet = _sheet;

        _sheet->setSkin(skinPtr(new skin("crystal")));

        layoutPtr lay(new verticalLayout());
        _sheet->setLayout(lay);

        _sheet->size = vec2(_viewport->getActualWidth(), _viewport->getActualHeight());
/*
        // Menu item - play button
        buttonPtr playButton(new button("playButton"));
        _sheet->addChild(playButton);
        playButton->setText("play");
        playButton->setCallback("onClick", boost::bind(&mainMenuState::_clickButton, this, RUN_PLAY));

        // Menu item - editor button
        buttonPtr editorButton(new button("editorButton"));
        _sheet->addChild(editorButton);
        editorButton->setText("editor");
        editorButton->setCallback("onClick", boost::bind(&mainMenuState::_clickButton, this, RUN_EDITOR));

        // Menu item - credits button
        buttonPtr creditsButton(new button("creditsButton"));
        _sheet->addChild(creditsButton);
        creditsButton->setText("credits");
        creditsButton->setCallback("onClick", boost::bind(&mainMenuState::_clickButton, this, RUN_CREDITS));

        // Add buttons to the application window
        lay->addWidget(playButton);
        lay->addWidget(editorButton);
        lay->addWidget(creditsButton);
        */
    }

    void mainMenuState::_destroyGUI()
    {
        _sheet.reset();
        _dispatchSheet.reset();
    }

    void mainMenuState::render()
    {
        // Render our gui sheet for given viewport
        _sheet->render(_viewport);
    }

    void mainMenuState::_clickButton(whatToRun r)
    {
        _whatToRun = r;
    }

    void mainMenuState::tick(stateManager &mgr)
    {
        // Update GUI
        _sheet->tick(_realTimer.delta());

        // If we have already selected need game state to run
        switch (_whatToRun)
        {
            case RUN_PLAY:
                //mgr.push(statePtr(new playState(_input, _soundSys, _sceneMgr, _viewport, _opts)));
            break;

            case RUN_EDITOR:
                //mgr.push(statePtr(new editorState(_input, _soundSys, _sceneMgr, _viewport, _opts)));
            break;

            case RUN_CREDITS:
            {
                //playState *s = new playState(_input, _soundSys, _sceneMgr, _viewport, _opts);

                // Already know what to load
               // s->loadLevel("levels/credits.lvl");
                //mgr.push(statePtr(s));
            }
            break;

            default:
            break;
        }

        _whatToRun = RUN_NOTHING;
    }

    bool mainMenuState::keyPressed(const OIS::KeyEvent &e)
    {
        return engine::gui::guiDisp::keyPressed(e);
    }

    bool mainMenuState::keyReleased(const OIS::KeyEvent &e)
    {
        return engine::gui::guiDisp::keyReleased(e);
    }

    bool mainMenuState::mouseMoved(const OIS::MouseEvent &e)
    {
        return engine::gui::guiDisp::mouseMoved(e);
    }

    bool mainMenuState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
    {
        return engine::gui::guiDisp::mousePressed(e, id);
    }

    bool mainMenuState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
    {
        return engine::gui::guiDisp::mouseReleased(e, id);
    }
}
