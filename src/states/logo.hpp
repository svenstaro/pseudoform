 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef LOGO_HPP_INCLUDED
#define LOGO_HPP_INCLUDED

#include "state.hpp"
#include "pre_ogre.hpp"
#include "types.hpp"
#include "time/timer.hpp"

namespace game
{

    class logoState : public state
    {
        private:

            // viewport for rendering
            Ogre::Viewport *_viewport;
            // timer & duration
            engine::time::timer _timer;
            engine::real _duration;

        public:

            logoState(
                Ogre::Viewport *viewport,
                engine::real dur = 1);

            ~logoState();

            void tick(stateManager &mgr);
            void render();

            void suspend();
            void resume();

    };

}

#endif // LOGO_HPP_INCLUDED
