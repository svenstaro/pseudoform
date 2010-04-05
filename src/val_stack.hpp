 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef VAL_STACK_HPP_INCLUDED
#define VAL_STACK_HPP_INCLUDED

#include <cassert>
#include <deque>

namespace engine
{
    /**
    * @class valStack
    * @brief Yet another realization of a stack
    */
    template<typename T>
    class valStack
    {
        private:

            typedef std::deque<T> stack;
            /// Deque of some T-style object
            stack _stack;

        public:
            /**
            * Constructor
            * @param val some value to push into the stack
            */
            valStack(const T &val)
            {
                _stack.push_back(val);
            }

            /**
            * @brief Overloaded operator ()
            *
            * We use it for taking last element from deque
            */
            T &operator()()
            {
                assert(!_stack.empty() && "Value stack must not be empty");
                return _stack.back();
            }

            /**
            * @brief Overlodaded operator[]
            * @param i value index
            *
            * Used for taking alement from deque by given index
            */
            T &operator[](size_t i)
            {
                assert(!_stack.empty() && "Value stack must not be empty");
                assert(_stack.size() > i && "Index of value stack too big");
                return _stack[i];
            }


            const T &operator()() const
            {
                assert(!_stack.empty() && "Value stack must not be empty");
                return _stack.back();
            }

            const T &operator[](size_t i) const
            {
                assert(!_stack.empty() && "Value stack must not be empty");
                assert(_stack.size() > i && "Index of value stack too big");
                return _stack[i];
            }

            /**
            * @brief Push element into deque
            * @param val element value to put
            * @return inserted element
            */
            const T &push(const T &val)
            {
                _stack.push_back(val);
                return val;
            }

            /**
            * @brief Pop element from deque
            */
            void pop()
            {
                _stack.pop_back();
            }
    };
}

#endif // VAL_STACK_HPP_INCLUDED
