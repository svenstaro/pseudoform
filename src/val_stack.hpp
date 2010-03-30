 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef VAL_STACK_HPP_INCLUDED
#define VAL_STACK_HPP_INCLUDED

#include <cassert>
#include <deque>

namespace engine
{

    template<typename T>
    class valStack
    {

        private:

            typedef std::deque<T> stack;
            stack _stack;

        public:

            valStack(const T &val)
            {
                _stack.push_back(val);
            }

            T &operator()()
            {
                assert(!_stack.empty() && "value stack must not be empty");
                return _stack.back();
            }

            T &operator[](size_t i)
            {
                assert(!_stack.empty() && "value stack must not be empty");
                assert(_stack.size() > i && "index of value stack too big");
                return _stack[i];
            }

            const T &operator()() const
            {
                assert(!_stack.empty() && "value stack must not be empty");
                return _stack.back();
            }

            const T &operator[](size_t i) const
            {
                assert(!_stack.empty() && "value stack must not be empty");
                assert(_stack.size() > i && "index of value stack too big");
                return _stack[i];
            }

            const T &push(const T &val)
            {
                _stack.push_back(val);
                return val;
            }

            void pop()
            {
                _stack.pop_back();
            }

    };

}

#endif // VAL_STACK_HPP_INCLUDED
