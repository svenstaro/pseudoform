 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef ACTION_BINDER_HPP_INCLUDED
#define ACTION_BINDER_HPP_INCLUDED

#include <map>
#include <utility>

#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>

#include "string.hpp"
#include "log.hpp"

/**
 * @namespace engine
 * @brief Consist of functions related to the game-engine
 */
namespace engine
{
    namespace input
    {
        class input;
    }

    /// Action responce
    typedef boost::function<void(const boost::any&)> response;
    /// Response map
    typedef std::map<string, response> responseMap;
    /// Action map
    typedef std::map<unsigned, response> actionMap;

    /**
     * @class actionBinder
     * @brief Connects actions with callback-functions(responses)
     *
     * The logic for this class is a bit confusing.
     * What actually ends up happening is this:
     *   1) _responses is filled with function bindings
     *   2) _pressed, _released, or _held are filled with
     *     lua-based bindings that in-turn call go() with
     *     the appropriate action
     */
    class actionBinder
    {
        protected:
			/// Map for different kind of actions
            actionMap _pressed, _released, _held;

            /// Responses for the created actions
            responseMap _responses;

            /**
             * @brief Get response for action
             * @name str action name
             */
            boost::optional<response> _response(const string &str) const
            {
                responseMap::const_iterator i = _responses.find(str);
                if (i != _responses.end())
                    return i->second;
                return boost::optional<response>();
            }

        public:
            /**
             * @brief Get response
             * @param str action name
             */
            void go(const string &str, const boost::any &misc);

            /**
             * @brief Bind press action at number `i` to the given response
             * @param i action index
             * @param r response to bind
             */
            void bindPressed(unsigned i, const response &r) { _pressed[i] = r; }

            /**
             * @brief Bind release action at number `i` to the given response
             * @param i action index
             * @param r response to bind
             */
            void bindReleased(unsigned i, const response &r) { _released[i] = r; }

            /**
             * @brief Bind held action at number `i` to the given response
             * @param i action index
             * @param r response to bind
             */
            void bindHeld(unsigned i, const response &r) { _held[i] = r; }

            /**
             * Bind some action due to this:
             * str[0] =
             * '+' pressed
             * '-' released
             * otherwise held
             * @param str action identifier
             * @param r response for given action
             */
            void bind(const string &str, const response &r);

            /**
             * @brief Unbind all actions
             */
            void unbindAll();

            // TODO: cooment updateHeld/Pressed/Released
            void updateHeld(const input::input &in);
            void updatePressed(unsigned c);
            void updateReleased(unsigned c);

            /**
             * Destructor
             */
            virtual ~actionBinder()
            {
            }
    };
}

#endif // ACTION_BINDER_HPP_INCLUDED
