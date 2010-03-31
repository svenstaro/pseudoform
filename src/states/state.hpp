 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef STATE_HPP_INCLUDED
#define STATE_HPP_INCLUDED

#include <map>
#include <deque>

#include <boost/shared_ptr.hpp>

#include "types.hpp"

/**
 * @namespace game
 * @brief Consist of tools for building game world
 */
namespace game
{
	/**
	 * @class stateManager
	 * @brief Manages switching between game states
	 */
    class stateManager;

    /**
     * @class state
     * @brief Base abstract state class
     * @see class stateManager
     */
    class state
    {
        public:
			/**
			 * Constructor
			 */
            state() { }

            /**
             * Destructor
             */
            virtual ~state() { }

            /**
             * @brief Game timer tick, used with global application cycle
             * @param mgr state manager handler
             */
            virtual void tick(stateManager &mgr) = 0;

            /**
             * @brief Render switched game state
             */
            virtual void render() = 0;

            /**
             * @brief State's render pause
             */
            virtual void suspend() = 0;

            /**
             * @brief Exit from render pause (run render)
             */
            virtual void resume() = 0;

    };

    /// Shared pointer for the state object
    typedef boost::shared_ptr<state> statePtr;

    /// Deque of states shared pointers
    typedef std::deque<statePtr> stateDeque;

    class stateManager
    {
        private:
			/// Stack of states to use in game
            stateDeque _stack;

            /// Pointer for the one state (for saving state)
            statePtr _tempState;

        public:
			/**
			 * Destructor
			 */
            ~stateManager();

            /**
             * @brief Push given state into state manager stack
             * @param p pointer to the state to put in
             */
            void push(const statePtr &p);

            /**
             * @brief Get pointer for the state at the top of stack
             * @return pointer to the state class
             */
            statePtr pop();

            /**
             * @brief Timer tick in render
             *
             * Implements virtual function from abstract state class.
             * Used for ticking time in render cycle
             */
            bool tick();

            /**
             * @brief Render given state
             */
            void render();
    };
}

#endif // STATE_HPP_INCLUDED
