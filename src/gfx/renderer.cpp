 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <stdexcept>

#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreRenderSystem.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreManualObject.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreGpuProgram.h>

#include "renderer.hpp"
#include "types.hpp"
#include "log.hpp"
#include "render_lib.hpp"

namespace engine
{
    namespace gfx
    {
        Ogre::RenderSystem *renderer::renderSys() const
        {
            return Ogre::Root::getSingleton().getRenderSystem();
        }

        void renderer::clearFrameBuffer(unsigned buffers, const colour &c,
            float d, unsigned short s) const
        {
            renderSys()->_setViewport(_viewport);
            renderSys()->clearFrameBuffer(buffers, c, d, s);
        }

//        void renderer::render(Ogre::SceneNode *node) const
//        {
//            node->_update(false, true);
//            mat4 world;
//            //world = mat4::getTrans(0, 0, 25);
//            world.makeTransform(
//                node->_getDerivedPosition(),
//                node->_getDerivedScale(),
//                node->_getDerivedOrientation());
//
//            renderSys()->_setWorldMatrix(world);
//
//            mat4 view = _camera->getViewMatrix(true);
//            renderSys()->_setViewMatrix(view);
//
//            mat4 proj = _camera->getProjectionMatrixWithRSDepth();
//            renderSys()->_setProjectionMatrix(proj);
//
//            // do rendering if visible
//            for (size_t i = 0; i < node->numAttachedObjects(); ++i)
//            {
//                Ogre::MovableObject *mo = node->getAttachedObject(i);
//
//                // only render entities for now
//                if (mo->getMovableType() != "Entity") continue;
//
//                Ogre::Entity *ent = static_cast<Ogre::Entity*>(mo);
//
//                // only render if visible
//                if (!ent->isVisible()) continue;
//
//                for (size_t j = 0; j < ent->getNumSubEntities(); ++j)
//                {
//                    Ogre::SubEntity *sub = ent->getSubEntity(j);
//
//                    Ogre::RenderOperation rop;
//                    sub->getRenderOperation(rop);
//
//                    Ogre::Pass *pass = sub->getMaterial()->getBestTechnique()->getPass(0);
//                    _sceneMgr->_setPass(pass, true, false);
//
//                    //renderSys()->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
//                    //renderSys()->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
//
//                    renderSys()->_render(rop);
//                }
//            }
//
//            for (size_t i = 0; i < node->numChildren(); ++i)
//                render(static_cast<Ogre::SceneNode*>(node->getChild(i)));
//        }

        idGen renderer::_idGen;

        renderer::renderer(Ogre::SceneManager *sceneMgr, Ogre::Viewport *viewport):
            _sceneMgr(sceneMgr),
            _viewport(viewport),
            _camera(_viewport->getCamera()),
            _quad(viewport->getActualWidth(), viewport->getActualHeight()),
            _name(_idGen("renderer")),
            _dt(0)
        {
            // Crossing cameras between scene managers is NOT GOOD!
            assert(_sceneMgr->hasCamera(_camera->getName()) &&
                "renderer's viewport's camera must belong to its sceneMgr");

            _quad.rect(vec4(0, 0, 1, 1), true);
        }

        renderer::~renderer()
        {
            _idGen -= _name;
        }

        void renderer::fsquad(Ogre::Pass *p, Ogre::Viewport *vp, bool beginEnd) const
        {
            _quad.render(p, vp, _camera, beginEnd);
        }

        void renderer::tick(real dt)
        {
            _dt = dt;
        }

//        void renderer::renderScene(Ogre::Camera *cam, Ogre::Viewport *vp, const preRenderFunc &prefix) const
//        {
//            prefix();
//            _sceneMgr->_renderScene(cam, vp, false);
//            renderSignal(*this, cam, vp, prefix);
//        }

        unsigned renderer::width() const
        {
            return _viewport->getActualWidth();
        }

        unsigned renderer::height() const
        {
            return _viewport->getActualHeight();
        }

        Ogre::RenderTarget *renderer::rt() const
        {
            return _viewport->getTarget();
        }
    }
}
