 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef TEXT_HPP_INCLUDED
#define TEXT_HPP_INCLUDED

#include <map>

#include <OgreCommon.h>

#include "panel.hpp"

namespace engine
{

    namespace gui
    {

        extern const string DEFAULT_FONT;

        class text;
        typedef boost::shared_ptr<text> textPtr;

        class text: public panel
        {

            public:

                struct glyphInfo
                {
                    size_t x, y, w, h;
                    bool valid;

                    glyphInfo(size_t x, size_t y, size_t w, size_t h):
                        x(x), y(y), w(w), h(h), valid(true) {}

                    glyphInfo(): x(0), y(0), w(1), h(1), valid(false) {}
                };

                const glyphInfo &info(unsigned k) const;

            protected:

                Ogre::Texture *_texture, *_fontTexture;
                Ogre::Font *_font;

                string _text;

                bool _dirty;

                typedef std::map<unsigned, glyphInfo> glyphInfoMap;
                glyphInfoMap _glyphs;
                void _updateGlyphs();

                void _renderText();
                void _resizeTexture();
                void _destroyTexture();
                void _loadFont(const string &str);

                void _renderThis(Ogre::Viewport *vp);

            public:

                vec2 textSize(const string &str) const;
                vec2 actualSize() const
                {
                    return textSize(_text);
                }
                vec2 textEnd(const string &str) const;

                text(const string &name, bool internal = false);
                ~text();

                void setText(const string &str);
                const string &getText() const { return _text; }
                void setFont(const string &str);
        };

    }

}

#endif // TEXT_HPP_INCLUDED
