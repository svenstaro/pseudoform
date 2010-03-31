 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

/*
 * Update list:
 * date
 * comments
 */

#ifndef BRUSH_HPP_INCLUDED
#define BRUSH_HPP_INCLUDED

#include "pre_ogre.hpp"

#include "types.hpp"
#include "id.hpp"

namespace engine
{
    namespace gui
    {
		/**
		 * Gui brush object
		 */
        class brush
        {
            private:
				/// Render system
                Ogre::RenderSystem *_rs;

                /// Passes for different states
                Ogre::Pass *_pass, *_selectPass, *_pressPass, *_noBlendPass;

                /// Quad for brush
                Ogre::ManualObject *_quad;

                /// Brush texture name
                string _texture;

                /// Opacity of brush
                float _alpha;

                /// Brush color
                colour _colour;

                /// Map of brushes names
                static idGen _idGen;

            public:
                /**
                 * Constructor
                 */
                brush();

                /**
                 * Destructor
                 */
                ~brush();

                /**
                 * Set texture for the brush
                 * @param str name of texture from resources
                 */
                void texture(const string &str) { _texture = str; }

                /**
                 * Set opacity of brush
                 * @param a brush opacity from 0.0 to 1.0(full opacity)
                 */
                void alpha(float a) { _alpha = a; }

                /**
                 * Brush color
                 * @param c color to use
                 */
                void colourMult(const ::engine::colour &c) { _colour = c; }

                /// Different kinds of effects for brush
                enum effect
                {
                    E_NONE, E_SELECT, E_PRESS, E_NO_BLEND
                };

                /**
                 * Render brush quad
                 * @param vp viewport to render in
                 * @param rect vector4 - 4 coordinates of rectangle to use brush in
                 * @param e use some effect for brush quad
                 */
                void render(Ogre::Viewport *vp, const vec4 &rect, const effect &e = E_NONE) const;
        };
    }
}


#endif // BRUSH_HPP_INCLUDED
