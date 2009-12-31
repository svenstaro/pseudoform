 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

namespace engine
{

    namespace input
    {
        class input;
    }

    typedef boost::function<void(const boost::any&)> response;
    typedef std::map<string, response> responseMap;
    typedef std::map<unsigned, response> actionMap;

    /*
        the logic for this class is a bit confusing...

        what actually ends up happening is this:

        1) _responses is filled with function bindings

        2) _pressed, _released, or _held are filled with
        lua-based bindings that in-turn call go() with
        the appropriate action
    */

    class actionBinder
    {
        protected:

            actionMap _pressed, _released, _held;

            responseMap _responses;

            boost::optional<response> _response(const string &str) const
            {
                responseMap::const_iterator i = _responses.find(str);
                if (i != _responses.end())
                    return i->second;
                return boost::optional<response>();
            }

        public:

            void go(const string &str, const boost::any &misc);

            void bindPressed(unsigned i, const response &r) { _pressed[i] = r; }
            void bindReleased(unsigned i, const response &r) { _released[i] = r; }
            void bindHeld(unsigned i, const response &r) { _held[i] = r; }

            void bind(const string &str, const response &r);
            void unbindAll();

            void updateHeld(const input::input &in);
            void updatePressed(unsigned c);
            void updateReleased(unsigned c);

            virtual ~actionBinder()
            {
            }
    };

}

#endif // ACTION_BINDER_HPP_INCLUDED
