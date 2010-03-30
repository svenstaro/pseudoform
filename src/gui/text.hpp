 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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
		/// Default font-name
        extern const string DEFAULT_FONT;

        class text;

        /// Shared pointer for the text object
        typedef boost::shared_ptr<text> textPtr;

        /**
         * Represents text in 3d-game
         */
        class text: public panel
        {
            public:
				/**
				 * Font render information
				 */
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
				/// Texture to use with font
                Ogre::Texture *_texture, *_fontTexture;

                /// Ogre font object
                Ogre::Font *_font;

                /// String data for the text label
                string _text;

                // TODO ?
                bool _dirty;

                /// Font information map
                typedef std::map<unsigned, glyphInfo> glyphInfoMap;
                glyphInfoMap _glyphs;

                /**
                 * Update font information
                 */
                void _updateGlyphs();

                /**
                 * Render text lable
                 */
                void _renderText();

                /**
                 * Resize font texture
                 */
                void _resizeTexture();

                /**
                 * Destroy font texture
                 */
                void _destroyTexture();

                /**
                 * Load given font
                 * @param str name of font to load
                 */
                void _loadFont(const string &str);

                /**
                 * Render text label by hands in given viewport
                 * @param vp viewport to use for render
                 */
                void _renderThis(Ogre::Viewport *vp);

            public:
				/**
				 * Calculate rectangle size of text data
				 */
                vec2 textSize(const string &str) const;

                /**
                 * Get text size of current text object
                 */
                vec2 actualSize() const
                {
                    return textSize(_text);
                }

                // TODO ?
                vec2 textEnd(const string &str) const;

                /**
                 * Constructor
                 */
                text(const string &name, bool internal = false);

                /**
                 * Destructor
                 */
                ~text();

                // Inherited...
                widget &update(const string &name, const boost::any &val);
                boost::any attrib(const string &name) const;
        };
    }
}

#endif // TEXT_HPP_INCLUDED
