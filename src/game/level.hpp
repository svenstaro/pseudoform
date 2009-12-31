 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

    class level
    {
        private:

            engine::gfx::renderer &_renderer;
            engine::snd::system &_soundSys;

            void _regFactories(bool inGame);

            // no copying!
            level(const level&);
            level &operator=(const level&);

        public:

            world gameWorld;
            engine::phys::world physWorld;
            entityFactory entFactory;
            engine::script::script script;
            boost::shared_ptr<engine::gui::sheet> guiSheet;

            engine::snd::system &soundSys() { return _soundSys; }

            engine::string scriptName;

            level(engine::gfx::renderer &renderer,
                engine::snd::system &sndSys, bool inGame);
            ~level();

            void load(const engine::string &fileName);
            void save(const engine::string &fileName);

            void reset();
            void clear();
            void start();

            void tick(float dt);
    };

    typedef boost::shared_ptr<level> levelPtr;
}

#endif // LEVEL_HPP_INCLUDED
