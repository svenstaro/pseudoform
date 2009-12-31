 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
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

        class brush
        {
            private:

                Ogre::RenderSystem *_rs;

                Ogre::Pass *_pass, *_selectPass, *_pressPass, *_noBlendPass;

                Ogre::ManualObject *_quad;

                string _texture;

                float _alpha;

                colour _colour;

                static idGen _idGen;

            public:

                brush();
                ~brush();

                void texture(const string &str) { _texture = str; }

                void alpha(float a) { _alpha = a; }

                void colourMult(const ::engine::colour &c) { _colour = c; }

                enum effect
                {
                    E_NONE, E_SELECT, E_PRESS, E_NO_BLEND
                };

                void render(Ogre::Viewport *vp, const vec4 &rect, const effect &e = E_NONE) const;
        };

    }


}


#endif // BRUSH_HPP_INCLUDED
