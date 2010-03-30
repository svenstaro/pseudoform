 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef CUSTOM_PARAMS_IDX_HPP_INCLUDED
#define CUSTOM_PARAMS_IDX_HPP_INCLUDED

namespace engine
{
    namespace gfx
    {
        enum
        {
            /// Per-object clipping plane
            OBJECT_CLIP_PLANE_IDX = 256,

            /// Per-object clipping plane, but used for the whole scene
            SCENE_CLIP_PLANE_IDX = 257
        };
    }
}

#endif // CUSTOM_PARAMS_IDX_HPP_INCLUDED
