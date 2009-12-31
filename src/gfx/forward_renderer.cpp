 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgreMaterialManager.h>
#include <OgreSceneManager.h>
#include <OgreViewport.h>

#include "forward_renderer.hpp"
#include "game/portal.hpp"
#include "scene_clipper.hpp"

namespace engine
{


    namespace gfx
    {

        forwardRenderer::forwardRenderer(Ogre::SceneManager *sceneMgr, Ogre::Viewport *viewport):
            renderer(sceneMgr, viewport)
        {
            _sceneMgr->setAmbientLight(colour::White);
            _viewport->setMaterialScheme("forward");
        }

        forwardRenderer::~forwardRenderer()
        {
        }

        void forwardRenderer::render() const
        {
            // simply set the material scheme to forward rendering
            Ogre::MaterialManager::getSingleton().setActiveScheme("forward");
            {
                // reset the clipping plane that may have been done
                // by the portals in the previous frame
                sceneClipper clipper(vec4(0, 0, 0, 1), _sceneMgr->getRenderQueue());

                _sceneMgr->_renderScene(_camera, _viewport, false);
            }

            //! explicitely render all of the portals
            //! could use a better method, but this is easiest and fastest
            game::portal::renderAllPortals(_camera, _viewport);
        }

        void forwardRenderer::component(const string &name, bool active)
        {
            // forward renderer doesn't have any components
        }


    }


}
