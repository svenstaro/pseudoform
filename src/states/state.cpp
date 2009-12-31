 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */


#include "log.hpp"

#include "state.hpp"

namespace game
{

    // if we are destroyed with states still on the stack
    // we need to close them peacefully!
    stateManager::~stateManager()
    {
        while (!_stack.empty())
        {
            _stack.pop_back();
        }
    }

    void stateManager::push(const statePtr &p)
    {
        if (!_stack.empty()) // tell previous state to suspend
            _stack.back()->suspend();

        _stack.push_back(p);
    }

    statePtr stateManager::pop()
    {
        if (_stack.empty())
        {
            return statePtr();
        }
        // store a pointer to it
        statePtr ptr = _stack.back();
        // pop it off the stack
        _stack.pop_back();

        // and then make sure the previous state resumes
        if (!_stack.empty()) // tell previous state to resume
            _stack.back()->resume();

        return ptr;
    }

    void stateManager::render()
    {
        assert(!_stack.empty() && "cannot render empty game state stack");

        _stack.back()->render();
    }

    bool stateManager::tick()
    {
        assert(!_stack.empty() && "cannot tick empty game state stack");

        _stack.back()->tick(*this);

        return !_stack.empty();
    }


}
