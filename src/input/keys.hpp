 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef KEYS_HPP_INCLUDED
#define KEYS_HPP_INCLUDED

#include <map>

#include "string.hpp"

namespace engine
{
    namespace input
    {

        typedef std::map<string, unsigned> strToKeyMap;
        typedef std::map<unsigned, string> keyToStrMap;
        extern strToKeyMap _strToKeyMap;
        extern keyToStrMap _keyToStrMap;

        inline string str(unsigned c)
        {
            keyToStrMap::const_iterator i = _keyToStrMap.find(c);
            if (i != _keyToStrMap.end())
                return i->second;
            return "undefined";
        }

        inline unsigned key(const string &s)
        {
            strToKeyMap::const_iterator i = _strToKeyMap.find(s);
            if (i != _strToKeyMap.end())
                return i->second;
            return 0;
        }

    }
}

#endif // KEYS_HPP_INCLUDED
