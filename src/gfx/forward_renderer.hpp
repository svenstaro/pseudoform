 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef FORWARD_RENDERER_HPP_INCLUDED
#define FORWARD_RENDERER_HPP_INCLUDED

#include "renderer.hpp"

namespace engine
{

    namespace gfx
    {


        class forwardRenderer : public renderer
        {

            public:

                forwardRenderer(Ogre::SceneManager *sceneMgr, Ogre::Viewport *viewport);
                virtual ~forwardRenderer();

                void component(const string &name, bool active);
                void render() const;


        };

    }


}

#endif // FORWARD_RENDERER_HPP_INCLUDED
