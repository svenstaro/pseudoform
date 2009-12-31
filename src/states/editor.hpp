 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef EDITOR_HPP_INCLUDED
#define EDITOR_HPP_INCLUDED

//#include <map>
#include <list>

#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include "time/timer.hpp"
#include "gfx/renderer.hpp"

#include "game/world.hpp"
#include "game/entity.hpp"
#include "game/generic_prop.hpp"

#include "gui/gui_disp.hpp"

#include "phys/world.hpp"
#include "phys/interacter.hpp"

#include "script/script.hpp"

#include "editor/selection_node.hpp"
#include "editor/camera_control.hpp"

#include "state.hpp"

#include "pre_ogre.hpp"
#include "types.hpp"
#include "event.hpp"
#include "action_binder.hpp"

#include "game/level.hpp"

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

    typedef std::vector<boost::shared_ptr<levelFormat::entity> > stateList;

    class editorState : public state, public engine::gui::guiDisp, public engine::actionBinder
    {
        public:

            typedef std::list<entityPtr> pickedEntList;

        private:

            const engine::input::input &_input;
            engine::snd::system &_soundSys;
            engine::gui::sheetPtr _sheet;
            // graphics-related
            Ogre::SceneManager *_sceneMgr;
            Ogre::Viewport *_viewport;
            engine::gfx::rendererPtr _renderer, _thumbnailRenderer;
            // timer
            engine::time::timer _realTimer, _timer;

            levelPtr _level;

            // all objects start out stopped
            bool _runningSimulation;

            void _createScript();
            void _destroyScript();

            editor::cameraControl _cameraControl;

            bool _rotateAsGroup; // per-object rotate or as a group?

            bool _shouldExit;

            interfaceType _spawnType;

            // whether we should start playing in-editor
            bool _startPlay;
            void _pushPlayState(stateManager &mgr);

            void _createGUI();
            void _destroyGUI();
//            void _setupResponses();

            // entity about to be spawned
            genericPropPtr _spawnEnt;
            // picked entities
            pickedEntList _pickedEnts;

            stateList _preTransformStates, _postTransformStates;
            void _preTransform();
            void _postTransform();

            // selection node for AABB and gizmos
            editor::selectionNodePtr _selectionNode;
            void _updateSelectionNode();
            void _selectionControl();

            // drag using middle-click when in simulation
            engine::phys::interacter _simInteracter;
            void _updateSimInteracter();

            void _clickLeft();
            void _releasePickedEnt(const entityPtr &ent);
            void _releasePickedEnts();
            void _pickEnt(float mx, float my, bool add = false);
            void _updatePropertiesWin();
            void _updateLevelPropertiesWin();
            void _destroySpawnEnt();
            void _updateSpawnEnt(float mx, float my);

            void _timeSpeedScroll(const engine::gui::event &e);
            void _mouseSpeedScroll(const engine::gui::event &e);

            engine::eventManager _eventMgr;
            void _doEvent(engine::event *e)
            {
                _eventMgr += engine::eventPtr(e);
            }

            editorState(const editorState &rhs); // no copying

        public:

            void play();
            void redo();
            void undo();
            void escape();
            void rotateAsGroup(bool b);
            void gizmoType(const engine::string &str);
            void gizmoVisible(int axis, bool b);
            void selectSpawnType(const interfaceType &type);
            void startSimulation();
            void stopSimulation();
            void toggleSimulation();
            void deletePicked();
            void duplicatePicked();
            void clear();
            void exit()
            {
                _shouldExit = true;
            }

            void loadLevel(const engine::string &str);
            void saveLevel(const engine::string &str);

            void createSpawnEnt(const engine::string &script);

            editorState(
                const engine::input::input &input,
                engine::snd::system &soundSys,
                Ogre::SceneManager *mgr, Ogre::Viewport *vp,
                // possibly load a level at startup or something
                engine::programOptions *opts = NULL);

            ~editorState();

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

#endif // EDITOR_HPP_INCLUDED
