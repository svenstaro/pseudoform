 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <cmath>

#include <OgreManualObject.h>
#include <OgreSceneManager.h>
#include <OgreRoot.h>
#include <OgreRenderSystem.h>
#include <OgreRenderOperation.h>

#include "quad.hpp"
#include "render_lib.hpp"

namespace engine
{



    namespace gfx
    {

        idGen quad::_idGen;

        quad::quad(size_t screenW, size_t screenH):
            _quad(NULL),
            _rect(0, 0, 1, 1),
            _relative(true),
            _name(_idGen("quad"))
//            preSize(mat4::IDENTITY),
//            preMove(mat4::IDENTITY),
//            postMove(mat4::IDENTITY),
//            textureMat(mat4::IDENTITY)
        {
            // just get some scene manager, doesn't matter which
            Ogre::SceneManagerEnumerator::SceneManagerIterator sceneIter =
                Ogre::Root::getSingleton().getSceneManagerIterator();
            assert(sceneIter.hasMoreElements());

            Ogre::SceneManager *sceneMgr = sceneIter.peekNextValue();

            _quad = sceneMgr->createManualObject(_name);

            build(screenW, screenH);
        }

        void quad::build(size_t screenW, size_t screenH)
        {
            _quad->clear();
            _quad->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
            {
                float w = screenW, h = screenH;

                // top-right
                _quad->position(vec3(1, 1, 0));
                _quad->textureCoord(vec2(1, 0));
                _quad->textureCoord(vec2(w, 0));

                // bottom-right
                _quad->position(vec3(1, -1, 0));
                _quad->textureCoord(vec2(1, 1));
                _quad->textureCoord(vec2(w, h));

                // bottom-left
                _quad->position(vec3(-1, -1, 0));
                _quad->textureCoord(vec2(0, 1));
                _quad->textureCoord(vec2(0, h));

                // top-left
                _quad->position(vec3(-1, 1, 0));
                _quad->textureCoord(vec2(0, 0));
                _quad->textureCoord(vec2(0, 0));

                // vertices are clockwise, make them anticlockwise
                _quad->quad(3, 2, 1, 0);
            }
            _quad->end();
        }

        quad::~quad()
        {
            if (_quad)
                _quad->_getManager()->destroyManualObject(_quad);
            _quad = NULL;

            _idGen -= _name;
        }

        void quad::render(Ogre::Pass *p, Ogre::Viewport *vp, Ogre::Camera *cam,
            bool beginEnd) const
        {
            vec4 r = _rect;
            if (!_relative)
            {
                // if not relative, make it relative and cache it
                float w = 1.0 / vp->getActualWidth();
                float h = 1.0 / vp->getActualHeight();
                // scale XY to [0..2], ZW to [0..1]
                r *= vec4(w * 2, h * 2, w, h);
                // move XY to [-1..1] and add [0..1] WH to XY
                r += vec4(-1 + r.z, -1 + r.w, 0, 0);
                // negate Y because 0 is at bottom
                r.y *= -1;
            }

            vec3 farCorner(1, 1, 1);
            if (cam) // corner #4 is top-right
                farCorner = cam->getViewMatrix(true) * cam->getWorldSpaceCorners()[4];

            Ogre::RenderSystem *renderSys = Ogre::Root::getSingleton().getRenderSystem();

            Ogre::RenderOperation rop;
            _quad->getSection(0)->getRenderOperation(rop);

            renderSys->_setViewport(vp); // render to said viewport

            // clear any matrices
            renderSys->_setWorldMatrix(mat4::IDENTITY);
            renderSys->_setViewMatrix(mat4::IDENTITY);
            renderSys->_setProjectionMatrix(mat4::IDENTITY);

            _quad->_getManager()->_setPass(p, true, false); // parse the pass

            // bind/unbind GPU programs

            // scale to dimensions
            mat4 mat(mat4::getScale(r.z, r.w, 1));
            // translate by XY
            mat = mat4::getTrans(r.x, r.y, 0) * mat;
            // translate by half-texel offsets if necessary
            if (renderLib == RL_D3D)
            {
                // get [-1..1] offsets
                float ox = 1.0 / vp->getTarget()->getWidth();
                float oy = 1.0 / vp->getTarget()->getHeight();
                // subtract from quad position
                mat = mat4::getTrans(-ox, oy, 0) * mat;
                // (using a positive Y offset because Y goes up
                // in this space)
            }

            if (vp->getTarget()->requiresTextureFlipping())
                // flip if needed for GL render targets
                mat = mat4::getScale(1, -1, 1) * mat;

            if (p->hasVertexProgram())
            {
                Ogre::GpuProgramParametersSharedPtr params = p->getVertexProgramParameters();

                if (params->_findNamedConstantDefinition("farCorner"))
                    params->setNamedConstant("farCorner",
                        vec4(farCorner.x, farCorner.y, farCorner.z, 1));

                if (params->_findNamedConstantDefinition("mat"))
                    params->setNamedConstant("mat", mat);

//                if (params->_findNamedConstantDefinition("textureMat"))
//                    params->setNamedConstant("textureMat", textureMat);

                renderSys->bindGpuProgramParameters(Ogre::GPT_VERTEX_PROGRAM, params);
            }
            else
            {
                renderSys->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
                // we still need render target flipping for GL, though
                // use the projection matrix for it (as well as texel offsets)
                //renderSys->_setProjectionMatrix(mat);
                renderSys->_setViewMatrix(mat);
                //renderSys->_setTextureMatrix(0, textureMat);
            }

            if (p->hasFragmentProgram())
            {
                Ogre::GpuProgramParametersSharedPtr params = p->getFragmentProgramParameters();

                if (params->_findNamedConstantDefinition("colour"))
                    params->setNamedConstant("colour", p->getDiffuse());

                if (params->_findNamedConstantDefinition("farCorner"))
                    params->setNamedConstant("farCorner", vec4(farCorner.x, farCorner.y, farCorner.z, 1));

                renderSys->bindGpuProgramParameters(Ogre::GPT_FRAGMENT_PROGRAM, params);
            }
            else
                renderSys->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);

            if (beginEnd)
                renderSys->_beginFrame();

            renderSys->_render(rop);

            if (beginEnd)
                renderSys->_endFrame();
        }


    }



}


