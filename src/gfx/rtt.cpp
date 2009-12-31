 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgreRenderTarget.h>
#include <OgreRenderTexture.h>
#include <OgreTexture.h>
#include <OgreTextureManager.h>
#include <OgreViewport.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreRoot.h>
#include <OgreRenderSystem.h>

#include "rtt.hpp"

namespace engine
{
    namespace gfx
    {

        rtt::~rtt()
        {
            if (_texture)
            {
                Ogre::TextureManager::getSingleton().remove(_texture->getName());
            }
            _texture = NULL;
            _viewport = NULL;
            _rt = NULL;
        }

        void rtt::clear(unsigned buffers, const colour &c, float d, unsigned s)
        {
            assert(_viewport);

            Ogre::RenderSystem *rs = Ogre::Root::getSingleton().getRenderSystem();

            rs->_setViewport(_viewport);
            rs->clearFrameBuffer(buffers, c, d, s);
        }

        rtt::rtt(const string &name, size_t w, size_t h, Ogre::PixelFormat pf, Ogre::Camera *cam):
            _texture(NULL),
            _viewport(NULL),
            _rt(NULL)
        {
            Ogre::TextureManager &tm = Ogre::TextureManager::getSingleton();

            _texture = Ogre::TexturePtr(tm.createManual(
                name, "data", Ogre::TEX_TYPE_2D, w, h, 0, pf, Ogre::TU_RENDERTARGET)).get();

            _rt = _texture->getBuffer()->getRenderTarget();
            _viewport = _rt->addViewport(cam);

            _rt->setAutoUpdated(false);
            _viewport->setClearEveryFrame(false);
            _viewport->setShadowsEnabled(false);
            _viewport->setOverlaysEnabled(false);
        }

    }
}
