 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef STATE_HPP_INCLUDED
#define STATE_HPP_INCLUDED

#include <map>
#include <deque>

#include <boost/shared_ptr.hpp>

#include "types.hpp"

namespace game
{

    class stateManager;
    class state
    {

        public:

            state()
            {
            }

            virtual ~state()
            {
            }

            virtual void tick(stateManager &mgr) = 0;
            virtual void render() = 0;

            virtual void suspend() = 0; // still on stack, but not the top (temporarily stop)
            virtual void resume() = 0; // still on stack, now the top (keep going)

    };

    typedef boost::shared_ptr<state> statePtr;

//    template<typename A, typename B>
//    boost::shared_ptr<A> stateCast(const boost::shared_ptr<B> &ptr)
//    {
//        return boost::static_pointer_cast<A>(ptr);
//    }

    typedef std::deque<statePtr> stateDeque;

    class stateManager
    {
        private:

            stateDeque _stack;

            statePtr _tempState;

        public:

            ~stateManager();

            // push a state onto the stack
            void push(const statePtr &p);
            // pop a state off the stack
            statePtr pop();

            bool tick();
            void render();
    };

    //typedef boost::shared_ptr<stateManager> stateManagerPtr;

}


#endif // STATE_HPP_INCLUDED
