// /*
// * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
// * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
// * For conditions of distribution and use, see copyright notice in COPYING
// */
//
//#include <OgreResourceGroupManager.h>
//
//#include "gui/panel.hpp"
//#include "gui/mouse.hpp"
//#include "gui/window.hpp"
//#include "gui/skin.hpp"
//#include "gui/sheet.hpp"
//
//#include "gfx/forward_renderer.hpp"
//#include "input/input.hpp"
//
//#include "gui/gui_disp.hpp"
//
//#include "snd/system.hpp"
//
//#include "game/generic_prop.hpp"
//#include "game/character.hpp"
//#include "game/portal.hpp"
//#include "game/trigger.hpp"
//#include "game/player.hpp"
//#include "data_dir.hpp"
//
//#include "play.hpp"
//#include "data_dir.hpp"
//#include "log.hpp"
//#include "types.hpp"
//#include "safe_any.hpp"
//#include "game/level.hpp"
//#include "game/marker.hpp"
//
//#include <OgreSceneManager.h>
//
//namespace game
//{
//
//    playState::playState(
//        const engine::input::input &input,
//        engine::snd::system &soundSys,
//        Ogre::SceneManager *mgr, Ogre::Viewport *vp,
//        engine::programOptions *opts):
//        _input(input),
//        _soundSys(soundSys),
//        _sceneMgr(mgr),
//        _viewport(vp),
//        _shouldExit(false)
//    {
//        using namespace engine;
//
//        _renderer.reset(new gfx::forwardRenderer(_sceneMgr, _viewport));
//
//        _level.reset(new level(*_renderer, _soundSys, true)); // in-game
//
//        _createScript();
//        _createGUI();
//
//        // create the character
//        {
//            playerPtr p = ENTITY_CAST(player, _level->entFactory.create(player::TYPE, "player"));
//            _level->gameWorld.add(p);
//        }
//
//        // route input to us
//        _input(this, this);
//
//        if (opts)
//        {
//            for (engine::programOptions::iterator i = opts->begin(); i != opts->end(); ++i)
//            {
//                const engine::string &opt = *i;
//
//                if (opt.find("-loadLevel") != engine::string::npos)
//                {
//                    engine::string fn = opt.substr(opt.find_first_of("=") + 1);
//                    loadLevel(fn);
//                }
//            }
//        }
//    }
//
//    // init from another state
//    playState::playState(const engine::input::input &input,
//        engine::snd::system &soundSys,
//        const engine::gfx::rendererPtr &renderer,
//        const levelPtr &lvl):
//        _input(input),
//        _soundSys(soundSys),
//        _sceneMgr(renderer->sceneMgr()),
//        _viewport(renderer->viewport()),
//        _renderer(renderer),
//        _level(lvl),
//        _shouldExit(false)
//    {
//        using namespace engine;
//
//        _createScript();
//        _createGUI();
//
//        // create the character
//        {
//            playerPtr p = ENTITY_CAST(player, _level->entFactory.create(player::TYPE, "player"));
//            p->pos(renderer->camera()->getDerivedPosition());
//            p->rot(renderer->camera()->getDerivedOrientation());
//            _level->gameWorld.add(p);
//        }
//
//        // route input to us
//        _input(this, this);
//
//        _level->start();
//    }
//
//    playState::~playState()
//    {
//        _destroyGUI();
//        _destroyScript();
//
//        _level->reset();
//
//        // remove player
//        // make sure this is done after the level reset
//        // because otherwise if the player is inside a portal
//        // the portal will cause a crash because the player
//        // is removed without it knowing
//        _level->gameWorld.remove("player");
//
//        // destroy renderer
//        _renderer.reset();
//    }
//
//    void playState::clear()
//    {
//        // save the player
//        entityPtr p = _level->gameWorld.get("player");
//        // clear the rest of the level
//        _level->clear();
//        // re-add the player
//        _level->gameWorld.add(p);
//    }
//
//    void playState::loadLevel(const engine::string &fn)
//    {
//        if (fn.empty())
//        {
//            engine::log("tried to load empty level");
//            return;
//        }
//        // clear current level
//        clear();
//        // load new level
//        _level->load(fn);
//        _level->start();
//
//        {
//            playerPtr p = ENTITY_CAST(player, _level->gameWorld.get("player"));
//            markerPtr spawnPoint = ENTITY_CAST(marker, _level->gameWorld.get("player_spawn"));
//            if (spawnPoint)
//                p->matrix(spawnPoint->matrix());
//        }
//    }
//
//    void playState::_createScript()
//    {
////        if (!_level->lua.file(engine::DATA_DIR + "scripts/play.lua"))
////        {
////            engine::log("scripts/play.lua error: ");
////            engine::log(_level->lua.error());
////        }
////
////        engine::lua::callFunc(_level->lua, "playMain", boost::ref(*_level), boost::ref(*this));
//    }
//
//    void playState::_destroyScript()
//    {
//        // lua will get auto-destructed by destructors
//
//        // however! key binds have boost::bind's to lua-functions
//        // this is BAD! if lua is destroyed before these binds,
//        // then some stray luabind::objects will be destroyed after
//        // lua.  this causes a very, VERY obscure crash.  so just
//        // get rid of the key binds before lua goes
//        unbindAll();
//    }
//
//    void playState::_createGUI()
//    {
//        using namespace engine::gui;
//        using engine::vec2;
//        using engine::string;
//
//        #ifndef DEBUG_NEWTON_BUILD
//        _sheet.reset(new sheet("editorState::_sheet"));
//        // let the dispatch use this sheet
//        _dispatchSheet = _sheet;
//        // let the level use our sheet
//        _level->guiSheet = _sheet;
//
//        // hide mouse
//        _sheet->mouse()->visible = false;
//
//        _sheet->setSkin(skinPtr(new skin("crystal")));
//
//        _sheet->size = vec2(_renderer->width(), _renderer->height());
//
////        engine::lua::callFunc(_level->lua, "createPlayGUI");
//
//        #endif
//    }
//
//    void playState::_destroyGUI()
//    {
//        _dispatchSheet.reset();
//        _sheet.reset();
//    }
//
//    void playState::suspend()
//    {
//    }
//
//    void playState::resume()
//    {
//        // route input to us
//        _input(this, this);
//
//        // reset timer to avoid any flukes
//        _timer.reset();
//
//        _level->guiSheet = _sheet;
//
//        #ifndef DEBUG_NEWTON_BUILD
//        // hide mouse
//        _sheet->mouse()->visible = false;
//        #endif
//    }
//
//    void playState::render()
//    {
//        _renderer->render();
//
//        #ifndef DEBUG_NEWTON_BUILD
//        // render our gui sheet
//        _sheet->render(_viewport);
//        #endif
//    }
//
//    void playState::_playerControl()
//    {
//        using namespace engine;
//
////        vec3 move(0, 0, 0);
////        if (_input.keys()->isKeyDown(OIS::KC_W))
////            move += engine::vec3(0, 0, -1);
////        if (_input.keys()->isKeyDown(OIS::KC_D))
////            move += engine::vec3(1, 0, 0);
////        if (_input.keys()->isKeyDown(OIS::KC_S))
////            move += engine::vec3(0, 0, 1);
////        if (_input.keys()->isKeyDown(OIS::KC_A))
////            move += engine::vec3(-1, 0, 0);
//
//        vec3 look = _input.mouseRel();
//
//        playerPtr p = ENTITY_CAST(player, _level->gameWorld.get("player"));
//
////        p->movement += move;
//        p->yaw(-look.x);
//        p->pitch(-look.y);
//    }
//
//    void playState::move(const engine::vec3 &v)
//    {
//        using namespace engine;
//
//        playerPtr p = ENTITY_CAST(player, _level->gameWorld.get("player"));
//
//        p->movement += v;
//    }
//
//    void playState::jump()
//    {
//        using namespace engine;
//        playerPtr p = ENTITY_CAST(player, _level->gameWorld.get("player"));
//        p->jump();
//    }
//
//    void playState::tick(stateManager &mgr)
//    {
//        _timer();
//
//        updateHeld(_input);
//
//        _playerControl();
//
//        _level->tick(_timer.delta());
//        #ifndef DEBUG_NEWTON_BUILD
//        // advance GUI
//        _sheet->tick(_timer.delta());
//        #endif
//
//        // move the camera appropriately
//        {
//            playerPtr p = ENTITY_CAST(player, _level->gameWorld.get("player"));
//            _renderer->camera()->setPosition(p->getCamPos());
//            _renderer->camera()->setOrientation(p->getCamRot());
//        }
//
//        // advance graphical effects
//        _renderer->tick(_timer.delta());
//        // set sound listener position
//        {
//            engine::quat rot = _renderer->camera()->getDerivedOrientation();
//            engine::vec3 pos = _renderer->camera()->getDerivedPosition();
//            _soundSys.listener(pos, -rot.zAxis(), rot.yAxis());
//        }
//
//        if (_shouldExit)
//            mgr.pop();
//    }
//
//    bool playState::keyPressed(const OIS::KeyEvent &e)
//    {
//        updatePressed(e.key);
//        return engine::gui::guiDisp::keyPressed(e);
//    }
//
//    bool playState::keyReleased(const OIS::KeyEvent &e)
//    {
//        updateReleased(e.key);
//        return engine::gui::guiDisp::keyReleased(e);
//    }
//
//    bool playState::mouseMoved(const OIS::MouseEvent &e)
//    {
//        return engine::gui::guiDisp::mouseMoved(e);
//    }
//
//    bool playState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
//    {
//        return engine::gui::guiDisp::mousePressed(e, id);
//    }
//
//    bool playState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
//    {
//        return engine::gui::guiDisp::mouseReleased(e, id);
//    }
//
//}
