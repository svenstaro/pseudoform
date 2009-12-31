 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef RENDER_LIB_HPP_INCLUDED
#define RENDER_LIB_HPP_INCLUDED

namespace engine
{
    namespace gfx
    {

        enum renderLib_t { RL_D3D, RL_OGL };
        extern renderLib_t renderLib;

    }
}

#endif // RENDER_LIB_HPP_INCLUDED
