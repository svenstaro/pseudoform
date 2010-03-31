 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef ID_HPP_INCLUDED
#define ID_HPP_INCLUDED

#include <map>

#include <OgreStringConverter.h>

#include "types.hpp"

/**
 * @namespace engine
 * @brief Consist of functions related to the game-engine
 */
namespace engine
{
	/**
	 * @class idGen
	 * @brief This class stores different stuff names
	 *
	 * We can use this class for storing ids for the objects like gui-stuff, ogre-stuff
	 */
    class idGen
    {
		private:
			/// Base type for the id maps
            typedef std::map<engine::string, engine::string> strMap;

            /// List of unique ids
            strMap _idMap;

        public:
			// TODO: commment idGen operator()
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

            /**
             * @brief Remove given id from the list
             * @param id id to remove
             */
            void operator -=(const engine::string &id)
            {
                strMap::iterator i = _idMap.find(id);
                if (i != _idMap.end())
                    _idMap.erase(i);
            }
    };
}

#endif // ID_HPP_INCLUDED
