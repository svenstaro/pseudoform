 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/filesystem.hpp>

#include <OgreTextureManager.h>
#include <OgreMaterialManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>

#include "utils.hpp"
#include "id.hpp"
#include "log.hpp"
#include "data_dir.hpp"

namespace engine
{
    namespace gfx
    {
        namespace
        {
            idGen id;
        }

        Ogre::Pass *getPass(const string &matName, size_t t, size_t p, bool clone)
        {
            if (!Ogre::MaterialManager::getSingleton().resourceExists(matName))
            {
                log("Could not find \"" + matName + "\" material");
                return NULL;
            }

            Ogre::MaterialPtr mat = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().load(
                matName, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
            assert(mat.get());

            static idGen id;

            if (clone)
                mat = mat->clone(id(matName));

            return mat->getTechnique(t)->getPass(p);
        }

        Ogre::Pass *getPass(const string &matName, size_t t, const string &passName, bool clone)
        {
            if (!Ogre::MaterialManager::getSingleton().resourceExists(matName))
            {
                log("could not find \"" + matName + "\" material");
                return NULL;
            }

            Ogre::MaterialPtr mat = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().load(
                matName, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
            assert(mat.get());

            static idGen id;

            if (clone)
                mat = mat->clone(id(matName));

            try
            {
                Ogre::Pass *ret = mat->getTechnique(t)->getPass(passName);
                return ret;
            }
            catch (...)
            {
                log("Could not find pass \"" + passName + "\" in material \"" + matName + "\"");
            }
            return NULL;
        }

        Ogre::TexturePtr getTexture(const string &name)
        {
            try
            {
                Ogre::TexturePtr tex = Ogre::TexturePtr(Ogre::TextureManager::getSingleton().load(name,
                    Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
                return tex;
            }
            catch(...)
            {
            }
            return Ogre::TexturePtr();
        }

        bool hasTexture(const string &name)
        {
            return getTexture(name).get();
        }

        bool hasTextureFile(const string &name)
        {
            return boost::filesystem::exists(DATA_DIR + name);
        }
    }
}
