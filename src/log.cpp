 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgreLogManager.h>

#include "log.hpp"

namespace engine
{
	/// Filename of the engine log
    const string LOG_NAME("engine.log");

    void log(const string &str)
    {
        ::Ogre::LogManager::getSingleton().getLog(LOG_NAME)->logMessage(str, Ogre::LML_CRITICAL);
    }
}
