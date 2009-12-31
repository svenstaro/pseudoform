 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef RTT_HPP_INCLUDED
#define RTT_HPP_INCLUDED

#include <OgrePixelFormat.h>

#include "pre_ogre.hpp"
#include "string.hpp"
#include "types.hpp"

namespace engine
{
    namespace gfx
    {

        class rtt
        {
            private:

                rtt(const rtt&);
                rtt &operator=(const rtt&);

            protected:

                Ogre::Texture *_texture;
                Ogre::Viewport *_viewport;
                Ogre::RenderTarget *_rt;

            public:

                rtt(const string &name, size_t w, size_t h, Ogre::PixelFormat pf,
                    Ogre::Camera *cam = NULL);
                ~rtt();

                Ogre::Texture *texture() const { return _texture; }
                Ogre::Viewport *viewport() const { return _viewport; }
                Ogre::RenderTarget *rt() const { return _rt; }

                operator bool() const { return _texture ? true : false; }
                bool operator !() const { return !_texture; }

                void clear(unsigned buffers, const colour &c = colour::Black, float d = 1, unsigned s = 0);
        };

    }
}

#endif // RTT_HPP_INCLUDED
