 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SCENE_CLIPPER_HPP_INCLUDED
#define SCENE_CLIPPER_HPP_INCLUDED

#include <OgreRenderQueue.h>

#include "types.hpp"

namespace engine
{
    namespace gfx
    {
        // clips the scene via shaders for as long as it lives
        class sceneClipper: public Ogre::RenderQueue::RenderableListener
        {
            private:

                engine::vec4 _plane;
                Ogre::RenderQueue *_queue;
                Ogre::RenderQueue::RenderableListener *_oldListener;

            public:

                sceneClipper(const engine::vec4 &p, Ogre::RenderQueue *queue);
                ~sceneClipper();

                bool renderableQueued(Ogre::Renderable *rend,
                    Ogre::uint8 groupID,
                    Ogre::ushort priority,
                    Ogre::Technique **ppTech,
                    Ogre::RenderQueue *pQueue);
        };
    }
}

#endif // SCENE_CLIPPER_HPP_INCLUDED
