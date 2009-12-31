 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
//#include <boost/lambda/lambda.hpp>

#include <OgreResourceGroupManager.h>
#include <OgreRenderTarget.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreLight.h>

#include "gui/mouse.hpp"
#include "gui/panel.hpp"
#include "gui/window.hpp"
#include "gui/gui_disp.hpp"
#include "gui/scrollbar.hpp"
#include "gui/text.hpp"

#include "gfx/forward_renderer.hpp"
#include "gfx/rtt.hpp"
#include "input/input.hpp"
#include "input/keys.hpp"
#include "snd/system.hpp"

#include "game/level.hpp"
#include "game/generic_prop.hpp"
#include "game/character.hpp"
#include "game/portal.hpp"
#include "game/trigger.hpp"
#include "game/player.hpp"
#include "states/play.hpp"

// for picking objects and stuff
#include "phys/ray.hpp"

#include "time/sleep.hpp"

#include "editor.hpp"
#include "data_dir.hpp"
#include "fs_ops.hpp"
#include "log.hpp"
#include "event.hpp"
#include "id.hpp"
#include "safe_any.hpp"

#include "editor/selection_node.hpp"

#include "protobuf/level_format.pb.h"

#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreRay.h>

namespace game
{

    class spawnEntEvent: public engine::event
    {
        private:

            entityPtr _ent;
            editorState::pickedEntList &_pickedEnts;
            bool _wasPicked;
            world &_gameWorld;

        public:

            spawnEntEvent(entityPtr ent, editorState::pickedEntList &pickedEnts, world &gameWorld):
                _ent(ent),
                _pickedEnts(pickedEnts),
                _wasPicked(false),
                _gameWorld(gameWorld)
            {
            }

            bool undo()
            {
                _gameWorld.remove(_ent->name());

                genericPropPtr gp = ENTITY_CAST(genericProp, _ent);
                gp->active(false);
                gp->visible(false);
                gp->aabbVisible(false);

                _wasPicked = (std::find(_pickedEnts.begin(), _pickedEnts.end(), _ent) != _pickedEnts.end());
                if (_wasPicked)
                    _pickedEnts.remove(_ent);

                return true;
            }

            bool redo()
            {
                if (!_ent)
                    return false;

                _gameWorld.add(_ent);

                _ent->reset(); // reset the entity

                genericPropPtr gp = ENTITY_CAST(genericProp, _ent);
                gp->active(true);
                gp->visible(true);
                if (_wasPicked)
                {
                    gp->aabbVisible(true);
                    _pickedEnts.push_back(_ent);
                }

                return true;
            }
    };

    class deletePickedEntsEvent: public engine::event
    {
        private:

            editorState::pickedEntList _ents, &_picked;
            world &_gameWorld;

        public:

            deletePickedEntsEvent(const editorState::pickedEntList &ents,
                editorState::pickedEntList &picked,
                world &gameWorld):
                _ents(ents),
                _picked(picked),
                _gameWorld(gameWorld)
            {
            }

            bool undo()
            {
                BOOST_FOREACH(entityPtr &ent, _ents)
                {
                    // add to the world
                    _gameWorld.add(ent);
                    // add to picked (they were originally picked)
                    _picked.push_back(ent);
                    // and reset it
                    ent->reset();

                    genericPropPtr gp = ENTITY_CAST(genericProp, ent);
                    gp->active(true);
                    gp->visible(true);
                    gp->aabbVisible(true);
                }

                return true;
            }

            bool redo()
            {
                BOOST_FOREACH(entityPtr &ent, _ents)
                {
                    // remove from world
                    _gameWorld.remove(ent->name());
                    // remove from picked entities
                    _picked.remove(ent);

                    genericPropPtr gp = ENTITY_CAST(genericProp, ent);
                    gp->active(false);
                    gp->visible(false);
                    gp->aabbVisible(false);
                }

                return true;
            }
    };

    class transformPickedEntsEvent: public engine::event
    {
        private:

            editorState::pickedEntList _picked;

            stateList _before, _after;

            world &_gameWorld;

        public:

            transformPickedEntsEvent(
                const editorState::pickedEntList &picked,
                const stateList &before,
                const stateList &after,
                world &gameWorld):
                _picked(picked),
                _before(before),
                _after(after),
                _gameWorld(gameWorld)
            {
            }

            bool undo()
            {
                size_t j = 0;
                BOOST_FOREACH(entityPtr &i, _picked)
                {
                    i->load(*_before[j]);
                    i->setCurrentStateAsInitial();
                    j++;
                }
                return true;
            }

            bool redo()
            {
                size_t j = 0;
                BOOST_FOREACH(entityPtr &i, _picked)
                {
                    i->load(*_after[j]);
                    i->setCurrentStateAsInitial();
                    j++;
                }
                return true;
            }
    };

    editorState::editorState(
        const engine::input::input &input,
        engine::snd::system &soundSys,
        Ogre::SceneManager *mgr, Ogre::Viewport *vp,
        engine::programOptions *opts):
        _input(input),
        _soundSys(soundSys),
        _sceneMgr(mgr),
        _viewport(vp),
        _runningSimulation(false),
        _cameraControl(input),
        _rotateAsGroup(false),
        _shouldExit(false),
        _spawnType(genericProp::TYPE),
        _startPlay(false)
    {
        using namespace engine;

        _renderer.reset(new gfx::forwardRenderer(_sceneMgr, _viewport));

        _level.reset(new game::level(*_renderer, _soundSys, false)); // not in-game

        _createScript();
        _createGUI();

        _selectionNode.reset(new editor::selectionNode(_sceneMgr->getRootSceneNode()));

        // route input to us
        _input(this, this);

//        _setupResponses();

        //! testing
        //_loadLevel(engine::DATA_DIR + "levels/phys_portals.lvl");
        if (opts)
        {
            for (engine::programOptions::iterator i = opts->begin(); i != opts->end(); ++i)
            {
                const engine::string &opt = *i;

                if (opt.find("-loadLevel") != engine::string::npos)
                {
                    engine::string level = opt.substr(opt.find_first_of("=") + 1);
                    loadLevel(level);
                    startSimulation();
                }
            }
        }
    }

    editorState::~editorState()
    {
        _selectionNode.reset();

        clear();

        _destroyGUI();
        _destroyScript();

        _level.reset();

        // destroy renderer
        _renderer.reset();
    }

    void editorState::suspend()
    {
    }

    void editorState::resume()
    {
        // route input to us
        _input(this, this);

        // reset timer to avoid any flukes
        _timer.reset();
        _realTimer.reset();

        _level->guiSheet = _sheet;

        // when we resume physics, we were probably in a play
        // state or something of the sort - reset the game world
//        _gameWorld->reset();
        // play state should reset the world for itself

        _cameraControl.reset();
    }

    void editorState::_createGUI()
    {
        using namespace engine;
        using namespace engine::gui;

        _sheet.reset(new sheet("editorState::_sheet"));
        // let the dispatch use this sheet
        _dispatchSheet = _sheet;
        _level->guiSheet = _sheet;

        _sheet->size = vec2(_renderer->width(), _renderer->height());

//        lua::callFunc(_level->lua, "createEditorGUI");

//        _sheet->childInFamily("timeSpeedScrollbar")
//            .callback("onScroll", boost::bind(&editorState::_timeSpeedScroll, this, _1));
//
//        _sheet->childInFamily("mouseSpeedScrollbar")
//            .callback("onScroll", boost::bind(&editorState::_mouseSpeedScroll, this, _1));
    }

    void editorState::_destroyGUI()
    {
        _sheet.reset();
        _dispatchSheet.reset();
    }

    void editorState::_timeSpeedScroll(const engine::gui::event &e)
    {
        using namespace engine;
        using namespace engine::gui;

        // percentage
        float f = SAFE_ANY_CAST_T(float, e.misc, 1);

        f = f * 2 - 1; // [0..1] -> [-1..0]

        _timer.speed = f;
        _soundSys.speed(f);
    }

    void editorState::selectSpawnType(const interfaceType &type)
    {
        using namespace engine;

        _spawnType = type;
    }

    void editorState::_mouseSpeedScroll(const engine::gui::event &e)
    {
        using namespace engine;
        using namespace engine::gui;

        // percentage
        float f = SAFE_ANY_CAST_T(float, e.misc, 1);

        _input.mouseSens = f * 0.25;
    }

    void editorState::_createScript()
    {
//        if (!_level->lua.file(engine::DATA_DIR + "scripts/editor.lua"))
//        {
//            engine::log("scripts/editor.lua error: ");
//            engine::log(_level->lua.error());
//        }
//
//        engine::lua::callFunc(_level->lua, "editorMain", boost::ref(*_level), boost::ref(*this));
    }

    void editorState::_destroyScript()
    {
        // lua will get auto-destructed by destructors

        // however! key binds have boost::bind's to lua-functions
        // this is BAD! if lua is destroyed before these binds,
        // then some stray luabind::objects will be destroyed after
        // lua.  this causes a very, VERY obscure crash.  so just
        // get rid of the key binds before lua goes
        unbindAll();
    }

    void editorState::_updateSelectionNode()
    {
        if (_pickedEnts.empty())
            _selectionNode->setVisible(false);
        else
        {
            _selectionNode->setVisible(true);

            using namespace engine;
            quat rot(quat::IDENTITY);

            aabb_t aabb; // AABB to draw
            vec3 pos(0, 0, 0); // average position for the widgets
            BOOST_FOREACH(entityPtr &i, _pickedEnts)
            {
                if (!i->hasInterface(genericProp::TYPE))
                    continue;

                genericPropPtr gp = ENTITY_CAST(genericProp, i);

                assert(gp);

                aabb.merge(gp->aabb());
                pos += gp->pos();

                rot = gp->rot();
            }
            pos /= _pickedEnts.size();

            _selectionNode->update(aabb, pos, rot);

            _selectionNode->keepSize(_renderer->camera()->getDerivedPosition());
        }
    }

    void editorState::_preTransform()
    {
        _preTransformStates.clear();
        _postTransformStates.clear();

        BOOST_FOREACH(entityPtr &i, _pickedEnts)
        {
            _preTransformStates.push_back(i->copyOfInitialState());
        }
    }

    void editorState::_postTransform()
    {
        if (_preTransformStates.empty())
            return;

        BOOST_FOREACH(entityPtr &i, _pickedEnts)
        {
            _postTransformStates.push_back(i->copyOfInitialState());
        }

        _doEvent(new transformPickedEntsEvent(
            _pickedEnts, _preTransformStates, _postTransformStates, _level->gameWorld));

        _preTransformStates.clear();
        _postTransformStates.clear();
    }

    void editorState::gizmoType(const engine::string &str)
    {
        if (str == "translate") _selectionNode->setGizmoType(editor::selectionNode::G_TRANSLATE);
        else if (str == "rotate") _selectionNode->setGizmoType(editor::selectionNode::G_ROTATE);
        else if (str == "scale") _selectionNode->setGizmoType(editor::selectionNode::G_SCALE);
        else return;

        // if we switched the gizmo type, then trigger post transform
        if (_input.isHeld(engine::input::MB_0))
        {
            _postTransform();
            _preTransform();
        }
    }

    void editorState::play()
    {
        _startPlay = true;
    }

    void editorState::_pushPlayState(stateManager &mgr)
    {
        _startPlay = false;

        // make sure we're not picking anything, etc.
        _releasePickedEnts();
        _destroySpawnEnt();
        _updateSelectionNode();

        mgr.push(statePtr(new playState(_input, _soundSys, _renderer, _level)));
    }

    void editorState::gizmoVisible(int axis, bool b)
    {
        _selectionNode->setGizmoVisible(axis, b);

        // if we turned off an axis, then we want to trigger postTransform
        if (!b && _input.isHeld(engine::input::MB_0))
        {
            _postTransform();
            // and if the gizmo is still active on another axis, re-do it
            if (_selectionNode->active())
                _preTransform();
        }
    }

    void editorState::loadLevel(const engine::string &str)
    {
        clear();

        // load new level
        _level->load(str);

        _updateLevelPropertiesWin();
    }

    void editorState::saveLevel(const engine::string &str)
    {
        _level->save(str);
    }

    void editorState::clear()
    {
        // get rid of the current spawn entity
        _spawnEnt.reset();

        // clear the transformation undo/redo parameters
        _preTransformStates.clear();
        _postTransformStates.clear();
        _eventMgr.clear();

        // we must release the current picked ents or we'll crash
        // when the new level loads
        _releasePickedEnts();
        _updatePropertiesWin();

        _level->clear();

        _updateLevelPropertiesWin();
    }

    void editorState::_updateLevelPropertiesWin()
    {
//        engine::lua::callFunc(_level->lua, "updateLevelPropertiesWin", _level->script);
    }

    void editorState::render()
    {
        _renderer->render();

        // render our gui sheet
        _sheet->render(_viewport);
    }

    void editorState::_selectionControl()
    {
        using namespace engine;

        if (!_input.isHeld(input::MB_0))
            return;

        _cameraControl.movedWithLeftMB = true;

        vec3 v = _input.mouseRel() * vec3(1, -1, 0.1);

        // transform into world space
        v = _renderer->camera()->getDerivedOrientation() * v;

        switch (_selectionNode->getGizmoType())
        {
            case editor::selectionNode::G_ROTATE:

                BOOST_FOREACH(entityPtr &i, _pickedEnts)
                {
                    genericPropPtr gp = ENTITY_CAST(genericProp, i);
                    assert(gp);

                    quat r = gp->rot();
                    vec3 p = gp->pos();

                    vec3 vec(_selectionNode->gizmoVec());

                    quat pitch(deg(vec.x * v.y), vec3(1, 0, 0));
                    quat yaw(deg(vec.y * v.x), vec3(0, 1, 0));
                    quat roll(deg(vec.z * v.x), vec3(0, 0, 1));
                    quat total = yaw * pitch * roll;
                    total.normalise();

                    r = total * r;
                    r.normalise();

                    if (_rotateAsGroup)
                    {
                        // we must move the objects around the group center
                        // if we're rotating the whole group
                        p -= _selectionNode->gizmoPos();
                        p = total * p;
                        p += _selectionNode->gizmoPos();
                    }

                    gp->rot(r);
                    gp->pos(p);
                    gp->setCurrentStateAsInitial();
                }

            break;

            case editor::selectionNode::G_TRANSLATE:

                BOOST_FOREACH(entityPtr &i, _pickedEnts)
                {
                    genericPropPtr gp = ENTITY_CAST(genericProp, i);
                    assert(gp);

                    quat r = gp->rot();
                    vec3 p = gp->pos();

                    p += _selectionNode->gizmoVec() * v * 0.05;

                    gp->pos(p);
                    gp->setCurrentStateAsInitial();
                }
            break;

            case editor::selectionNode::G_SCALE:

                // local transformation space for scaling
                v = _selectionNode->gizmoRot().UnitInverse() * v;

                BOOST_FOREACH(entityPtr &i, _pickedEnts)
                {
                    genericPropPtr gp = ENTITY_CAST(genericProp, i);
                    assert(gp);

                    vec3 s = gp->scale();

                    s += _selectionNode->gizmoVec() * v * 0.0125;

                    gp->scale(s);
                    gp->setCurrentStateAsInitial();
                }
            break;
        }

        // make mouse invisible
        _cameraControl.timeLeftToVisibleMouse = 0.25;
    }

    void editorState::_updateSpawnEnt(float mx, float my)
    {
        using namespace engine;

        if (!_spawnEnt)
            return;

        // get the mouse ray
        Ogre::Ray mouseRay = _renderer->camera()->getCameraToViewportRay(mx, my);

        const float MAX_DIST = 1000;
        phys::ray::hitList hits = phys::ray()(_level->physWorld, mouseRay.getOrigin(), mouseRay.getPoint(MAX_DIST));

        if (hits.empty())
            return;

        // we want first hit
        const phys::ray::hitInfo &hit = hits.front();

        vec3 y = hit.n;
        vec3 z = y.perpendicular();
        vec3 x = z.crossProduct(y);

        aabb_t aabb = _spawnEnt->aabb();

        _spawnEnt->pos(hit.p + aabb.getHalfSize().y * y);
        _spawnEnt->rot(quat(x, y, z));
    }

    void editorState::_updateSimInteracter()
    {
        engine::vec2 p = _sheet->mousePos01();
        Ogre::Ray ray = _renderer->camera()->getCameraToViewportRay(p.x, p.y);

        _simInteracter(ray.getOrigin(), ray.getDirection());
    }

    void editorState::tick(stateManager &mgr)
    {
        _timer();
        _realTimer();

        //_updateFPS();

        // update actions for held keys
        if (!_sheet->active())
        {
            updateHeld(_input);

            if (!_selectionNode->active())
                _cameraControl(_renderer->camera(), _realTimer.delta());
            else
                _selectionControl();

//            engine::log(Ogre::StringConverter::toString(_renderer->camera()->getDerivedPosition()) + "\n");
        }

        // selected objects will move
        _updateSelectionNode();

        _sheet->mouse()->visible = _cameraControl.timeLeftToVisibleMouse < 0;

        if (_runningSimulation)
        {
            // fix simulation interacter
            _updateSimInteracter();
            _level->tick(_timer.delta());
        }
        // advance lua
//        engine::lua::callFunc(_lua, "tick", _timer.delta(), _realTimer.delta());
        // advance GUI
        _sheet->tick(_realTimer.delta());
        // advance graphical effects
        _renderer->tick(_realTimer.delta());
        // set sound listener position
        {
            engine::quat rot = _renderer->camera()->getDerivedOrientation();
            engine::vec3 pos = _renderer->camera()->getDerivedPosition();
            _soundSys.listener(pos, -rot.zAxis(), rot.yAxis());
        }

        if (_startPlay)
            _pushPlayState(mgr);

        if (_shouldExit)
        {
            mgr.pop();
            return;
        }

        // we're an editor, so give some time to the OS
        //engine::time::sleep(8);
    }

    void editorState::redo()
    {
        _eventMgr.redo();
        //if (!_eventMgr.redo())
        //    engine::log("failed to redo() event");
    }

    void editorState::undo()
    {
        _eventMgr.undo();
        //if (!_eventMgr.undo())
        //    engine::log("failed to undo() event");
    }

    void editorState::startSimulation()
    {
        _runningSimulation = true;
        _timer.reset();

//        _gameWorld->reset();

        _level->start();

//        resume();
    }

    void editorState::stopSimulation()
    {
        _simInteracter.disable();

        _runningSimulation = false;
        _level->reset();

//        _updateSelectionNode();
    }

    void editorState::toggleSimulation()
    {
        if (_runningSimulation)
            stopSimulation();
        else
            startSimulation();
    }

    void editorState::escape()
    {
        if (_spawnEnt)
            _destroySpawnEnt();
        else if (!_pickedEnts.empty())
            _releasePickedEnts();
    }

    void editorState::rotateAsGroup(bool b)
    {
        _rotateAsGroup = b;
    }

    void editorState::deletePicked()
    {
        if (!_pickedEnts.empty())
        {
            BOOST_FOREACH(entityPtr &ent, _pickedEnts)
            {
                genericPropPtr gp = ENTITY_CAST(genericProp, ent);
                assert(gp);

                gp->visible(false);
                gp->active(false);
                gp->aabbVisible(false);

                // remove from world
                _level->gameWorld.remove(ent->name());
            }

            pickedEntList oldPickedEnts = _pickedEnts;

            // no more picked (until undone)
            _pickedEnts.clear();

            // avoid any undoing on deleted entities
            _preTransformStates.clear();
            _postTransformStates.clear();

            _doEvent(new deletePickedEntsEvent(oldPickedEnts, _pickedEnts, _level->gameWorld));

            _updatePropertiesWin();
        }

//        _updateSelectionNode();
    }

    void editorState::_clickLeft()
    {
        if (_sheet->active() || _selectionNode->active())
            // GUI click, ignore it
            return;

        if (_spawnEnt)
        {
            // clone the spawn entity
//            entityPtr ent = _spawnEnt->clone("ent");
            entityPtr ent = _level->entFactory.create(_spawnType, "ent");

            assert(ent);

            genericPropPtr gp = ENTITY_CAST(genericProp, ent);
            assert(gp);

//            gp->fromGenericProp(*_spawnEnt);
//            gp->setCurrentStateAsInitial();
            gp->loadAsInitialState(*_spawnEnt->currentState());

            if (gp->ent() && !gp->origMaterial().empty())
            {
                gp->ent()->setMaterialName(gp->origMaterial());
            }

            _level->gameWorld.add(ent);

//            _doEvent(new spawnEntEvent(ent, _pickedEnts, *_gameWorld));
        }
        else
        {
            engine::vec2 p = _sheet->mousePos01();
            // pick entity, and add it to the end of the list if ctrl is held
            _pickEnt(p.x, p.y, _input.mod(engine::input::M_CTRL));
        }
    }

    void editorState::createSpawnEnt(const engine::string &script)
    {
        using namespace engine;

        _spawnEnt.reset();

        entityPtr ent = _level->entFactory.create(genericProp::TYPE, "spawnEnt");
        _spawnEnt = ENTITY_CAST(genericProp, ent);
        assert(_spawnEnt);

        _spawnEnt->loadScript(script);

        _spawnEnt->active(false);

        if (_spawnEnt->ent())
        {
            _spawnEnt->ent()->setMaterialName("spawn_ent_template");
            _spawnEnt->ent()->setVisibilityFlags(engine::gfx::OVERLAY_GROUP);
        }
    }

    void editorState::_releasePickedEnt(const entityPtr &ent)
    {
        if (!ent)
            return;

        genericPropPtr gp = ENTITY_CAST(genericProp, ent);
        assert(gp);

        gp->aabbVisible(false);

        _pickedEnts.remove(ent);

        _updatePropertiesWin();
    }

    void editorState::_releasePickedEnts()
    {

        BOOST_FOREACH(entityPtr &ent, _pickedEnts)
        {
            genericPropPtr gp = ENTITY_CAST(genericProp, ent);
            assert(gp);

            gp->aabbVisible(false);
        }

        _pickedEnts.clear();

//        _updateSelectionNode();
        _updatePropertiesWin();
    }

    void editorState::duplicatePicked()
    {
        pickedEntList duplicated;

        BOOST_FOREACH(entityPtr &ent, _pickedEnts)
        {
            genericPropPtr gp = ENTITY_CAST(genericProp, ent);
            assert(gp);

            gp->aabbVisible(false);

            entityPtr copy = ent->clone(ent->name() + "_copy");
            _level->gameWorld.add(copy);
            duplicated.push_back(copy);

            gp = ENTITY_CAST(genericProp, copy);
            assert(gp);

            gp->aabbVisible(true);
        }

        _pickedEnts = duplicated;

        _updatePropertiesWin();
    }

    void editorState::_pickEnt(float mx, float my, bool add)
    {
        if (!add) // if not adding to the list, then remove others
            _releasePickedEnts();

        using namespace engine;
        // get the mouse ray
        Ogre::Ray mouseRay = _renderer->camera()->getCameraToViewportRay(mx, my);

        const float MAX_DIST = 1000;
        phys::ray::hitList hits = phys::ray()(_level->physWorld, mouseRay.getOrigin(), mouseRay.getPoint(MAX_DIST));

        if (hits.empty())
            return;

        // we want first hit
        const phys::ray::hitInfo &hit = hits.front();

        if (hit.b->misc.empty())
        {
            // hit a physics body without an entity, just get rid of this
            return;
        }

        entity *ent = SAFE_ANY_CAST_T(entity*, hit.b->misc, NULL);
        if (!ent)
        {
            log("picked body with no entity, odd");
            return;
        }

        entityPtr entPtr = ent->smartPtr();
        if (!entPtr)
        {
            log("picked entity not added to a world");
            return;
        }

        _pickedEnts.push_back(entPtr);

        genericPropPtr gp = ENTITY_CAST(genericProp, entPtr);
        assert(gp);

        gp->aabbVisible(true);

//        _updateSelectionNode();
        _updatePropertiesWin();
    }

    void editorState::_updatePropertiesWin()
    {
        using namespace engine::gui;
        using namespace engine;

        widgetPtr win = _sheet->child("propertiesWin");

        if (!win)
            return;

//        if (_pickedEnts.empty())
//            editor::propertiesWin::empty(win);
//        else if (_pickedEnts.size() > 1)
//            editor::propertiesWin::multipleSelection(win);
//        else
//            editor::propertiesWin::update(win, _pickedEnts.front().get());

        if (_pickedEnts.size() != 1)
        {
            win->clearChildren();
            win->resetLayout();
        }
        else
        {
//            engine::lua::callFunc(_level->lua, "updatePropertiesWin", _pickedEnts.front());
//            // we're using smart pointers so make sure lua cleans up properly
//            _level->lua.gc();
        }
    }

    void editorState::_destroySpawnEnt()
    {
        _spawnEnt.reset();
    }

    bool editorState::keyPressed(const OIS::KeyEvent &e)
    {
        if (!_sheet->active())
        {
            updatePressed(e.key);

            if (_input.keys()->isModifierDown(OIS::Keyboard::Ctrl))
            {
                switch (e.key)
                {
                    case OIS::KC_Z:
                        undo();
                    break;

                    case OIS::KC_Y:
                        redo();
                    break;

                    case OIS::KC_D:
                        duplicatePicked();
                    break;

                    default:
                    break;
                };
            }
        }

        return engine::gui::guiDisp::keyPressed(e);
    }

    bool editorState::keyReleased(const OIS::KeyEvent &e)
    {
        if (!_sheet->active())
        {
            updateReleased(e.key);
        }

        return engine::gui::guiDisp::keyReleased(e);
    }

    bool editorState::mouseMoved(const OIS::MouseEvent &e)
    {
        if (!_sheet->active())
        {
            engine::vec2 p = _sheet->mousePos01();
            // update where the spawn entity should appear
            _updateSpawnEnt(p.x, p.y);
        }

        return engine::gui::guiDisp::mouseMoved(e);
    }

    bool editorState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
    {
        if (!_sheet->active())
        {
            updatePressed(engine::input::convert(id));
            if (id == OIS::MB_Left)
            {
                // we could be transform stuff, save the state
                _preTransform();
            }
            else if (id == OIS::MB_Middle)
            {
                if (_runningSimulation)
                    _simInteracter.enable(_level->physWorld);
            }
        }

        return engine::gui::guiDisp::mousePressed(e, id);
    }

    bool editorState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
    {
        if (!_sheet->active())
        {
            updateReleased(engine::input::convert(id));
            if (id == OIS::MB_Left)
            {
                // if it's the left button and we didn't move, then
                // we must be picking/clicking on/placing an object
                if (!_cameraControl.movedWithLeftMB)
                {
                    _clickLeft();
                }
                else if (_selectionNode->active())
                {
                    // possibly stopped transforming
                    _postTransform();
                }

                _cameraControl.movedWithLeftMB = false;
            }
            else if (id == OIS::MB_Middle)
            {
                if (_runningSimulation)
                    _simInteracter.disable();
            }
        }

        return engine::gui::guiDisp::mouseReleased(e, id);
    }

}
