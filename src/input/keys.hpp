 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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
		/// Associative array which converts like "string" -> OIS value
        typedef std::map<string, unsigned> strToKeyMap;

        /// Associative array which converts like OIS value -> string
        typedef std::map<unsigned, string> keyToStrMap;

        /// Map of string OIS-keys definitions
        extern strToKeyMap _strToKeyMap;

        /// Map of native OIS-keys definitions
        extern keyToStrMap _keyToStrMap;

        /**
         * @brief Get string representation of key code
         * @param c key code to convert in a string
         * @return string representation of code
         */
        inline string str(unsigned c)
        {
            keyToStrMap::const_iterator i = _keyToStrMap.find(c);
            if (i != _keyToStrMap.end())
                return i->second;
            return "undefined";
        }

        /**
         * @brief Get key code from string
         * @param s string which includes key code
         * @return key code
         */
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
