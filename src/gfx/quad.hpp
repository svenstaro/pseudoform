 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef QUAD_HPP_INCLUDED
#define QUAD_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include <OgreColourValue.h>

#include "pre_ogre.hpp"
#include "id.hpp"
#include "types.hpp"

namespace engine
{

    namespace gfx
    {

        // shader-based quad rendering code (for post processing, etc.)
        class quad
        {
            private:

                Ogre::ManualObject *_quad;

                // rectangle xywh
                vec4 _rect;
                // relative to screen or pixel based?
                bool _relative;

                string _name;
                static idGen _idGen;

            public:

                // matrices at pre scale, pre move, and post all
//                mat4 preSize, preMove, postMove, textureMat;

                quad(size_t screenW = 1, size_t screenH = 1);
                ~quad();

                void build(size_t screenW, size_t screenH);

                // camera is for corner calculations if wanted
                void render(Ogre::Pass *p,
                    Ogre::Viewport *vp,
                    Ogre::Camera *cam = NULL,
                    bool beginEnd = true) const;

                void rect(const vec4 &r, bool isRelative = false)
                {
                    _rect = r;
                    _relative = isRelative;
                }
        };

        typedef boost::shared_ptr<quad> quadPtr;

    }

}


#endif // QUAD_HPP_INCLUDED
