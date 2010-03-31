 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef FAKE_MRL_HPP_INCLUDED
#define FAKE_MRL_HPP_INCLUDED

#include <OgreResource.h>

/**
 * @namespace engine
 * This namespace consist of functions related to the game-engine
 */

namespace engine
{
	// TODO: wtf
    class fakeMRL_t: public Ogre::ManualResourceLoader
    {
        public:
            void prepareResource(Ogre::Resource *p) {}
            void loadResource(Ogre::Resource *p) {}
    } fakeMRL;
    // a fake resource loader to suppress annoying Ogre log
    // that spams "no manual resource loader :( :( :("
}

#endif // FAKE_MRL_HPP_INCLUDED
