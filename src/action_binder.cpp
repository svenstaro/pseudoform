 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/foreach.hpp>

#include "action_binder.hpp"
#include "input/keys.hpp"
#include "input/input.hpp"

namespace engine
{

    void actionBinder::go(const string &str, const boost::any &misc)
    {
        boost::optional<response> r = _response(str);
        if (r)
            (*r)(misc);
        else
            log("invalid go() action: " + str);
    }

    void actionBinder::bind(const string &str, const response &r)
    {
        // + pressed
        // - released
        // otherwise held

        switch (str[0])
        {
            case '+': bindPressed(input::key(str.substr(1)), r); break;
            case '-': bindReleased(input::key(str.substr(1)), r); break;
            default: bindHeld(input::key(str), r); break;
        }
    }

    void actionBinder::unbindAll()
    {
        _pressed.clear();
        _released.clear();
        _held.clear();
    }

    void actionBinder::updateHeld(const input::input &in)
    {
        BOOST_FOREACH(actionMap::value_type &i, _held)
        {
            if (in.isHeld(i.first))
                i.second(boost::any());
        }
    }

    void actionBinder::updatePressed(unsigned c)
    {
        actionMap::iterator i = _pressed.find(c);
        if (i != _pressed.end())
            i->second(boost::any());
    }

    void actionBinder::updateReleased(unsigned c)
    {
        actionMap::iterator i = _released.find(c);
        if (i != _released.end())
            i->second(boost::any());
    }
}
