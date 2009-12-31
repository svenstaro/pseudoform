 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include "pre_ogre.hpp"

#include <OgreTexture.h>

#include "types.hpp"

namespace engine
{
    namespace gfx
    {
        Ogre::Pass *getPass(const string &matName, size_t t, size_t p, bool clone = false);
        Ogre::Pass *getPass(const string &matName, size_t t, const string &passName, bool clone = false);
        Ogre::TexturePtr getTexture(const string &name);
        bool hasTexture(const string &name);
        bool hasTextureFile(const string &name);
    }
}

#endif // UTILS_HPP_INCLUDED
