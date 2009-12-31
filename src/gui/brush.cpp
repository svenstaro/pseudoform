 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgreRenderSystem.h>
#include <OgreManualObject.h>
#include <OgreSceneManager.h>
#include <OgreRoot.h>

#include "brush.hpp"
#include "log.hpp"

#include "gfx/utils.hpp"
#include "gfx/render_lib.hpp"

namespace engine
{
    namespace gui
    {

        idGen brush::_idGen;

        brush::brush():
            _rs(Ogre::Root::getSingleton().getRenderSystem()),
            _pass(NULL),
            _selectPass(NULL),
            _pressPass(NULL),
            _noBlendPass(NULL),
            _quad(NULL)
        {
            _pass = gfx::getPass("gui_mat", 0, 0, true);
            _selectPass = gfx::getPass("gui_mat", 0, 1, true);
            _pressPass = gfx::getPass("gui_mat", 0, 2, true);
            _noBlendPass = gfx::getPass("gui_mat", 0, 3, true);

            if (!_pass || !_selectPass || !_pressPass || !_noBlendPass)
            {
                log("gui_mat unavailable, GUI rendering will be disabled");
            }

            // just get some scene manager, doesn't matter which
            Ogre::SceneManagerEnumerator::SceneManagerIterator sceneIter =
                Ogre::Root::getSingleton().getSceneManagerIterator();
            assert(sceneIter.hasMoreElements());

            Ogre::SceneManager *sceneMgr = sceneIter.peekNextValue();

            _quad = sceneMgr->createManualObject(_idGen("brush") + "::_quad");

            _quad->clear();
            _quad->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
            {
                // top-right
                _quad->position(vec3(1, 1, 0));
                _quad->textureCoord(vec2(1, 0));

                // bottom-right
                _quad->position(vec3(1, -1, 0));
                _quad->textureCoord(vec2(1, 1));

                // bottom-left
                _quad->position(vec3(-1, -1, 0));
                _quad->textureCoord(vec2(0, 1));

                // top-left
                _quad->position(vec3(-1, 1, 0));
                _quad->textureCoord(vec2(0, 0));

                // vertices are clockwise, make them anticlockwise
                _quad->quad(3, 2, 1, 0);
            }
            _quad->end();
        }

        brush::~brush()
        {
            if (_quad)
                _quad->_getManager()->destroyManualObject(_quad);
            _quad = NULL;
            _rs = NULL;
        }

//        void brush::prepareForUse() const
//        {
//            assert(_rs);
//
//            _rs->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
//            _rs->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
//
//            _rs->_setWorldMatrix(mat4::IDENTITY);
//            _rs->_setViewMatrix(mat4::IDENTITY);
//            _rs->_setProjectionMatrix(mat4::IDENTITY);
//
//            _rs->setLightingEnabled(false);
//            _rs->_setDepthBias(false);
//            _rs->_setDepthBufferCheckEnabled(false);
//            _rs->_setDepthBufferWriteEnabled(false);
//            _rs->_setColourBufferWriteEnabled(true, true, true, true);
//            _rs->_setFog(Ogre::FOG_NONE);
//            _rs->_setPolygonMode(Ogre::PM_SOLID);
//            _rs->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
//            _rs->_setTextureCoordSet(0, 0);
//            _rs->_setTextureUnitFiltering(0, Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);
//        }

//        void brush::texture(const string &name) const
//        {
////            assert(_rs);
//            if (!_pass)
//                return;
//
////            _rs->_setTexture(0, true, name);
//            _pass->getTextureUnitState(0)->setTextureName(name);
//        }

        void brush::render(Ogre::Viewport *vp, const vec4 &rect, const effect &e) const
        {
            if (_texture.empty())
                return;

            Ogre::Pass *pass = NULL;

            switch (e)
            {
                case E_NONE:
                    pass = _pass;
                break;

                case E_SELECT:
                    pass = _selectPass;
                break;

                case E_PRESS:
                    pass = _pressPass;
                break;

                case E_NO_BLEND:
                    pass = _noBlendPass;
                break;
            }

            if (!pass)
                return;

            pass->getTextureUnitState(0)->setTextureName(_texture);
//            if (!_pass)
//                _pass = gfx::getPass("gui_mat", 0, 0, true);
//            if (!_pass)
//                return;

            _quad->_getManager()->_setPass(pass, true, false);

            vec4 r = rect;
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

            Ogre::RenderOperation rop;
            _quad->getSection(0)->getRenderOperation(rop);

            _rs->_setViewport(vp); // render to said viewport

            // scale to dimensions
            mat4 mat(mat4::getScale(r.z, r.w, 1));
            // translate by XY
            mat = mat4::getTrans(r.x, r.y, 0) * mat;

            // translate by half-texel offsets if necessary
            if (gfx::renderLib == gfx::RL_D3D)
            {
                // get [-1..1] offsets
                float ox = 1.0 / vp->getTarget()->getWidth();
                float oy = 1.0 / vp->getTarget()->getHeight();
                // subtract from quad position
                mat = mat4::getTrans(-ox, oy, 0) * mat;
                // (using a positive Y offset because Y goes up
                // in this space)
            }

            // flip if needed for GL render targets
            if (vp->getTarget()->requiresTextureFlipping())
                mat = mat4::getScale(1, -1, 1) * mat;

            Ogre::GpuProgramParametersSharedPtr vertParams = pass->getVertexProgramParameters();

            if (vertParams->_findNamedConstantDefinition("mat"))
                vertParams->setNamedConstant("mat", mat);

            if (vertParams->_findNamedConstantDefinition("invSize"))
            {
                Ogre::TexturePtr tex = gfx::getTexture(_texture);
                if (tex.get())
                {
                    vec4 v(1.0 / tex->getWidth(), 1.0 / tex->getHeight(), 1, 1);
                    vertParams->setNamedConstant("invSize", v);
                }
            }

            _rs->bindGpuProgramParameters(Ogre::GPT_VERTEX_PROGRAM, vertParams);

            Ogre::GpuProgramParametersSharedPtr fragParams = pass->getFragmentProgramParameters();

            if (fragParams->_findNamedConstantDefinition("alpha"))
                fragParams->setNamedConstant("alpha", _alpha);

            if (fragParams->_findNamedConstantDefinition("colour"))
                fragParams->setNamedConstant("colour", _colour);

            if (fragParams->_findNamedConstantDefinition("invSize"))
            {
                Ogre::TexturePtr tex = gfx::getTexture(_texture);
                if (tex.get())
                {
                    vec4 v(1.0 / tex->getWidth(), 1.0 / tex->getHeight(), 1, 1);
                    fragParams->setNamedConstant("invSize", v);
                }
            }

            _rs->bindGpuProgramParameters(Ogre::GPT_FRAGMENT_PROGRAM, fragParams);

            _rs->_beginFrame();
            _rs->_render(rop);
            _rs->_endFrame();
        }

    }
}
