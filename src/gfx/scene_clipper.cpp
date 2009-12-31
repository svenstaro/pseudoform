 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgreRenderable.h>
#include <OgreTechnique.h>

#include "scene_clipper.hpp"
#include "custom_params_idx.hpp"

namespace engine
{
    namespace gfx
    {
        sceneClipper::sceneClipper(const engine::vec4 &p, Ogre::RenderQueue *queue):
            _plane(p),
            _queue(queue),
            _oldListener(_queue->getRenderableListener())
        {
            _queue->setRenderableListener(this);
        }

        sceneClipper::~sceneClipper()
        {
            _queue->setRenderableListener(_oldListener);
        }

        bool sceneClipper::renderableQueued(Ogre::Renderable *rend,
            Ogre::uint8 groupID,
            Ogre::ushort priority,
            Ogre::Technique **ppTech,
            Ogre::RenderQueue *pQueue)
        {
            rend->setCustomParameter(engine::gfx::SCENE_CLIP_PLANE_IDX, _plane);
            return true;
        }
    }
}
