 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef LEVEL_HPP_INCLUDED
#define LEVEL_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include "types.hpp"

#include "world.hpp"
#include "entity.hpp"
#include "phys/world.hpp"
#include "script/script.hpp"

namespace engine
{
    namespace gfx
    {
        class renderer;
    }

    namespace snd
    {
        class system;
    }

    namespace gui
    {
        class sheet;
    }
}

namespace game
{
	/**
	 * @class level
	 * @brief Represents game level
	 * @see class world
	 */
    class level
    {
        private:
			/// Renderer to use
            engine::gfx::renderer &_renderer;
            /// Sound system handler
            engine::snd::system &_soundSys;

            /**
             * @brief Register all need factories
             */
            void _regFactories(bool inGame);

            /**
             * Constructor
             * @remarks no copying constructor
             */
            level(const level&);

            /**
             * @brief Apply another level
             * @remarks exists due to absence of copying operator
             */
            level &operator=(const level&);

        public:
            /// Level game world
            world gameWorld;

            /// Level physics world
            engine::phys::world physWorld;

            /// Entity factory
            entityFactory entFactory;

            /// Script system handler
            engine::script::script script;

            /// Gui sheet handler
            boost::shared_ptr<engine::gui::sheet> guiSheet;

            /**
             * @brief Get level sound system handler
             */
            engine::snd::system &soundSys() { return _soundSys; }

            /// Script to use
            engine::string scriptName;

            /**
             * Constructor
             */
            level(engine::gfx::renderer &renderer,
                engine::snd::system &sndSys, bool inGame);

            /**
             * Destructor
             */
            ~level();

            /**
             * @brief Load game level
             * @param fileName file name that consist of level data
             */
            void load(const engine::string &fileName);

            /**
             * @brief Save world state into level format
             * @param fileName file name to save in
             */
            void save(const engine::string &fileName);

            /**
             * @brief Reset level state
             */
            void reset();

            /**
             * @brief Clear level
             */
            void clear();

            /**
             * @brief Start playing level
             */
            void start();

            /// Used with global application cycle
            void tick(float dt);
    };

    /**
     * Shared pointer to the current class
     */
    typedef boost::shared_ptr<level> levelPtr;
}

#endif // LEVEL_HPP_INCLUDED
