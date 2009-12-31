 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef ID_HPP_INCLUDED
#define ID_HPP_INCLUDED

#include <map>

//#include <boost/lexical_cast.hpp>
#include <OgreStringConverter.h>

#include "types.hpp"


namespace engine
{

    class idGen
    {
        private:

            typedef std::map<engine::string, engine::string> strMap;
            strMap _idMap;

        public:

            engine::string operator()(engine::string id, size_t n = 0)
            {
                strMap::iterator i = _idMap.find(id);
                if (i == _idMap.end())
                {
                    _idMap[id] = id;
                    return id;
                }

                if (n != 0)
                {
                    id = id.substr(0, id.find_last_of('_'));
                }
                id += "_" + Ogre::StringConverter::toString(n);

                return operator()(id, n + 1);
            }

            void operator -=(const engine::string &id)
            {
                strMap::iterator i = _idMap.find(id);
                if (i != _idMap.end())
                    _idMap.erase(i);
            }

    };

}


#endif // ID_HPP_INCLUDED
