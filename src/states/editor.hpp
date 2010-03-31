 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef EDITOR_HPP_INCLUDED
#define EDITOR_HPP_INCLUDED

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

// TODO: SCRIPTS
//#include "lua/lua.hpp"
//#include "script/script.hpp"

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

/**
 * @namespace game
 * @brief Consist of tools for building game world
 */
namespace game
{
	/// List of entities states
    typedef std::vector<boost::shared_ptr<levelFormat::entity> > stateList;

    /**
     * @class editorState
     * @brief Represents editor game mode
     * @see class state
     */
    class editorState : public state, public engine::gui::guiDisp, public engine::actionBinder
    {
        public:
			/// List to the shared pointer of picked enteties
            typedef std::list<entityPtr> pickedEntList;

        private:
			/// Input handler
            const engine::input::input &_input;
            /// Sound system handler
            engine::snd::system &_soundSys;
            /// Gui handler
            engine::gui::sheetPtr _sheet;
            /// Chosen scene manager for current state
            Ogre::SceneManager *_sceneMgr;
            /// Viewport to render in
            Ogre::Viewport *_viewport;
            /// Pointer to the need renderer
            engine::gfx::rendererPtr _renderer, _thumbnailRenderer;
            /// Game timers
            engine::time::timer _realTimer, _timer;

            /// Pointer to the chosen level
            levelPtr _level;

            /// All objects start out stopped
            bool _runningSimulation;

            /**
             * @brief Create script system, load editor-related bindings
             */
            void _createScript();
            /**
             * @brief Destroy script system
             */
            void _destroyScript();

            /// Camera controller
            editor::cameraControl _cameraControl;

            // Per-object rotate or as a group?
            bool _rotateAsGroup;

            /// Exit?
            bool _shouldExit;

            // TODO: interface type _spawnType
            interfaceType _spawnType;

            /// Whether we should start playing in-editor
            bool _startPlay;
            /**
             * Begin to play in-editor
             */
            void _pushPlayState(stateManager &mgr);

            /**
             * @brief Load gui
             */
            void _createGUI();
            /**
             * @brief Destroy gui
             */
            void _destroyGUI();
//            void _setupResponses();

            /// Entity about to be spawned
            genericPropPtr _spawnEnt;
            /// List of picked entities
            pickedEntList _pickedEnts;

            /// List of available states to store transformation
            stateList _preTransformStates, _postTransformStates;
            void _preTransform();
            void _postTransform();

            /// Selection node for AABB and gizmos
            editor::selectionNodePtr _selectionNode;
            void _updateSelectionNode();
            void _selectionControl();

            /// Drag using middle-click when in simulation
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

            /**
             * @brief Event time speed changed
             */
            void _timeSpeedScroll(const engine::gui::event &e);

            /**
             * @brief Event mose speed changed
             */
            void _mouseSpeedScroll(const engine::gui::event &e);

            engine::eventManager _eventMgr;
            void _doEvent(engine::event *e)
            {
                _eventMgr += engine::eventPtr(e);
            }

            editorState(const editorState &rhs); // no copying

        public:
			/**
			 * @brief Begin playing in edit mode
			 */
            void play();

            /**
             * @brief Redo event
             */
            void redo();

            /**
             * @brief Undo event
             */
            void undo();
            void escape();

            /**
             * @brief Rotate chosen entities as group
             * @param b use group rotating?
             */
            void rotateAsGroup(bool b);

            /**
             * @brief Set up gizmo type
             * @param str gizmo doing
             */
            void gizmoType(const engine::string &str);

            /**
             * @brief Set vizible some axis
             * @param axis to make visible
             * @param b make it really visible?
             */
            void gizmoVisible(int axis, bool b);

            // TODO: selectSpawnType
            void selectSpawnType(const interfaceType &type);

            /**
             * @brief Begin to simulate physics in editor mnode. Non game!
             */
            void startSimulation();

            /**
             * @brief Stop physics simulation in editor mode
             */
            void stopSimulation();

            /**
             * @brief Toggle physics simulation
             */
            void toggleSimulation();

            /**
             * @brief Delete picked entity from scene
             */
            void deletePicked();

            /**
             * @brief Duplicate picked entity
             */
            void duplicatePicked();

            /**
             * @brief Clear world
             */
            void clear();

            /**
             * @brief Exit from editor state
             */
            void exit()
            {
                _shouldExit = true;
            }

            /**
             * @brief Load level with given name
             * @param str file name with level to load in
             */
            void loadLevel(const engine::string &str);
            /**
             * @brief Save world state into level format
             * @param str file name to write in
             */
            void saveLevel(const engine::string &str);

            // TODO: comment crateSpawnEnt
            void createSpawnEnt(const engine::string &script);

            /**
             * Constructor
             */
            editorState(
                const engine::input::input &input,
                engine::snd::system &soundSys,
                Ogre::SceneManager *mgr, Ogre::Viewport *vp,
                engine::programOptions *opts = NULL);

            /**
             * Destructor
             */
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
