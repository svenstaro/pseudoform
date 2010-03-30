 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef RENDER_LIB_HPP_INCLUDED
#define RENDER_LIB_HPP_INCLUDED

namespace engine
{
    namespace gfx
    {
		/**
		 * @enum renderLib_t
		 * @brief List of available render libs
		 */
        enum renderLib_t { RL_D3D, RL_OGL };

        /// Here we store chosen render library
        extern renderLib_t renderLib;
    }
}

#endif // RENDER_LIB_HPP_INCLUDED
