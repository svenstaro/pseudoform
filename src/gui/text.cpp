 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <cstdlib>

#include <vector>

#include <boost/foreach.hpp>

#include <OgreTexture.h>
#include <OgreFont.h>
#include <OgreFontManager.h>
#include <OgreTextureManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreCommon.h>
#include <OgreHardwarePixelBuffer.h>

#include "text.hpp"
#include "log.hpp"
#include "safe_any.hpp"
#include "fake_mrl.hpp"
#include "string.hpp"

namespace engine
{

    namespace gui
    {

        const string DEFAULT_FONT("courier");

        text::text(const string &name, bool internal):
            panel(name, internal),
            _texture(NULL),
            _fontTexture(NULL),
            _font(NULL),
            _dirty(false)
        {
            _interfaces.push_back(WIDGET_TEXT);

            setFont(DEFAULT_FONT);
            decorate = false;
            clickable = false;
        }

        text::~text()
        {
            _destroyTexture();
        }

        void text::_loadFont(const string &str)
        {
            Ogre::FontPtr ptr = (Ogre::FontPtr)Ogre::FontManager::getSingleton().load(
                str, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
            if (!ptr.get())
            {
                log("failed to _loadFont() \"" + str + "\"");
                return;
            }

            _font = ptr.get();

            Ogre::MaterialPtr mat = _font->getMaterial();

            Ogre::Technique *t = mat->getTechnique(0);
            Ogre::Pass *p = t->getPass(0);
            Ogre::TextureUnitState *tu = p->getTextureUnitState(0);

            string texName = tu->getTextureName();
            _fontTexture = Ogre::TexturePtr(Ogre::TextureManager::getSingleton().getByName(texName)).get();

            _updateGlyphs(); // update the infos for the glyphs so we don't query the
            // font all the time
        }

        void text::_updateGlyphs()
        {
            assert(_font);

            _glyphs.clear();

            const Ogre::Font::CodePointRangeList &rngs = _font->getCodePointRangeList();

            BOOST_FOREACH(const Ogre::Font::CodePointRangeList::value_type &i, rngs)
            {
                // less than or equal to the last, or just less then?
                for (unsigned j = i.first; j <= i.second; ++j)
                {
                    Ogre::TRect<float> uv = _font->getGlyphTexCoords(j);

                    glyphInfo &gi = _glyphs[j];
                    gi.x = size_t(uv.left * _fontTexture->getWidth());
                    gi.y = size_t(uv.top * _fontTexture->getHeight());
                    gi.w = size_t(uv.width() * _fontTexture->getWidth());
                    gi.h = size_t(uv.height() * _fontTexture->getHeight());
                    gi.valid = true;
                }
            }
        }

        const text::glyphInfo &text::info(unsigned k) const
        {
            static glyphInfo invalid;
            glyphInfoMap::const_iterator i = _glyphs.find(k);
            if (i != _glyphs.end())
                return i->second;
            return invalid;
        }

        void text::_destroyTexture()
        {
            if (_texture)
            {
                Ogre::TextureManager::getSingleton().remove(_texture->getName());
                _texture = NULL;
            }
        }

        vec2 text::textSize(const string &str) const
        {
            if (!_font || str.empty())
                return vec2(0, 0);

            const glyphInfo &base = info(unsigned('X'));
            size_t x = 0, w = 0, h = base.h;

            BOOST_FOREACH(char c, str)
            {
                if (c == '\n')
                {
                    h += base.h;
                    x = 0; // start at front again
                }
                else if (c == '\t')
                    x += base.w * 4;
                else if (c == ' ')
                    x += base.w;
                else
                {
                    const glyphInfo &i = info(unsigned(c));
                    if (!i.valid) continue;
                    x += i.w;
                }

                // newlines cause us to start at the beginning
                // but we might already have a bigger width before
                w = std::max(w, x);
            }

            return vec2(w, h);
        }

        vec2 text::textEnd(const string &str) const
        {
            if (!_font || str.empty())
                return vec2(0, 0);

            const glyphInfo &base = info(unsigned('X'));
            size_t x = 0, y = 0;

            BOOST_FOREACH(char c, str)
            {
                if (c == '\n')
                {
                    y += base.h;
                    x = 0; // start at front again
                }
                else if (c == '\t')
                    x += base.w * 4;
                else if (c == ' ')
                    x += base.w;
                else
                {
                    const glyphInfo &i = info(unsigned(c));
                    if (!i.valid) continue;
                    x += i.w;
                }
            }

            return vec2(x, y);
        }

        void text::_resizeTexture()
        {
            if (!_font)
            {
                log("tried to _resizeTexture() with no font loaded on \"" + _name + "\"");
                return;
            }

            if (_text.empty())
                return;

            vec2 s = textSize(_text);

            assert(size_t(s.x) && size_t(s.y) && "text size cannot be 0, check textSize()");

            _destroyTexture();

            _texture = Ogre::TexturePtr(Ogre::TextureManager::getSingleton().createManual(
                fullName() + "::_texture", "data", Ogre::TEX_TYPE_2D, size_t(s.x), size_t(s.y), 0, // 0 mip maps
                _fontTexture->getFormat(), Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE, &fakeMRL)).get();
            // make sure that our panel renderer knows about our texture
            textureName(_texture->getName());
            size = s;
        }

        void text::_renderThis(Ogre::Viewport *vp)
        {
            // if dirty, then fix ourselves up
            if (_dirty)
            {
                _resizeTexture();
                _renderText();
                _dirty = false;
            }

            if (!_text.empty())
                panel::_renderThis(vp);
        }

        void text::_renderText()
        {
            if (_text.empty())
                return;

            if (!_font)
            {
                log("tried to _renderText() with no font loaded on \"" + _name + "\"");
                return;
            }

            Ogre::HardwarePixelBufferSharedPtr dest = _texture->getBuffer();
            Ogre::HardwarePixelBufferSharedPtr src = _fontTexture->getBuffer();

            // we want to clear the buffer to get rid of any garbage
            //if (false)
            {
                void *data = dest->lock(Ogre::HardwareBuffer::HBL_DISCARD);
                // make it all [0,0,0,0] (0 alpha)
                std::memset(data, 0, dest->getSizeInBytes());
                dest->unlock();
            }

            const glyphInfo &base = info(unsigned('X'));

            size_t x = 0, y = 0;
            BOOST_FOREACH(char c, _text)
            {
                if (c == '\n')
                {
                    y += base.h;
                    // start at left again
                    x = 0;
                }
                else if (c == '\t')
                    x += base.w * 4;
                else if (c == ' ')
                    x += base.w;
                else
                {
                    const glyphInfo &i = info(unsigned(c));
                    if (!i.valid) continue;

                    Ogre::Image::Box srcBox(i.x, i.y, i.x + i.w, i.y + i.h);
                    Ogre::Image::Box destBox(x, y, x + i.w, y + i.h);

                    dest->blit(src, srcBox, destBox);

                    // advance width
                    x += i.w;
                }
            }
        }

        void text::setFont(const string &str)
        {
            _loadFont(str);
            _dirty = true;
            // fix our size
            size = textSize(_text);
        }

        void text::setText(const string &str)
        {
            _text = str;
            _dirty = true;
            // fix our size
            size = textSize(_text);
        }

    }


}
