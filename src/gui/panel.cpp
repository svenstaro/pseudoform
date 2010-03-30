 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

/*
 * Update list:
 * date
 */

#include <boost/foreach.hpp>

#include <OgrePass.h>
#include <OgreTextureUnitState.h>
#include <OgreTexture.h>
#include <OgreTextureManager.h>

#include "panel.hpp"
#include "safe_any.hpp"
#include "brush.hpp"
#include "skin.hpp"

#include "gfx/utils.hpp"

namespace engine
{

    namespace gui
    {

        panel::panel(const string &name, bool internal):
            widget(name, internal)
        {
        }

        panel::~panel()
        {
        }

        string panel::getSkinType() const
        {
            return "panel";
        }

        void panel::_renderThis(Ogre::Viewport *vp)
        {
            widget::_renderThis(vp);

            brush *bru = getBrush();
            assert(bru);

            bru->alpha(derivedAlpha());
            bru->colourMult(_colour);

            vec2 dp = derivedPosition();

            if (decorate)
            {
                if (skin *s = getSkin())
                {
                    const string TYPE = getSkinType();

                    const skin::piece &tl = s->getPiece(TYPE, A_TL);
                    const skin::piece &tr = s->getPiece(TYPE, A_TR);
                    const skin::piece &t = s->getPiece(TYPE, A_T);

                    float topSize = std::max(std::max(tl.size.y, tr.size.y), t.size.y);

                    const skin::piece &bl = s->getPiece(TYPE, A_BL);
                    const skin::piece &br = s->getPiece(TYPE, A_BR);
                    const skin::piece &b = s->getPiece(TYPE, A_B);

                    const skin::piece &l = s->getPiece(TYPE, A_L);
                    const skin::piece &r = s->getPiece(TYPE, A_R);

                    float leftSize = std::max(std::max(tl.size.x, bl.size.x), l.size.x);

                    const skin::piece &mi = s->getPiece(TYPE, A_MI);

                    if (!tl.tex.empty())
                    {
                        bru->texture(tl.tex);
                        bru->render(vp, vec4(dp.x, dp.y, tl.size.x, tl.size.y));
                    }

                    if (!tr.tex.empty())
                    {
                        bru->texture(tr.tex);
                        bru->render(vp, vec4(dp.x + size.x + leftSize, dp.y, tr.size.x, tr.size.x));
                    }

                    if (!t.tex.empty())
                    {
                        bru->texture(t.tex);
                        bru->render(vp, vec4(dp.x + leftSize, dp.y, size.x, t.size.y));
                    }

                    if (!bl.tex.empty())
                    {
                        bru->texture(bl.tex);
                        bru->render(vp, vec4(dp.x, dp.y + size.y + topSize, bl.size.x, bl.size.y));
                    }

                    if (!br.tex.empty())
                    {
                        bru->texture(br.tex);
                        bru->render(vp, vec4(dp.x + size.x + leftSize, dp.y + size.y + topSize, br.size.x, br.size.x));
                    }

                    if (!b.tex.empty())
                    {
                        bru->texture(b.tex);
                        bru->render(vp, vec4(dp.x + leftSize, dp.y + size.y + topSize, size.x, b.size.y));
                    }

                    if (!l.tex.empty())
                    {
                        bru->texture(l.tex);
                        bru->render(vp, vec4(dp.x, dp.y + topSize, l.size.x, size.y));
                    }

                    if (!r.tex.empty())
                    {
                        bru->texture(r.tex);
                        bru->render(vp, vec4(dp.x + size.x + leftSize, dp.y + topSize, r.size.x, size.y));
                    }

                    if (!mi.tex.empty())
                    {
                        bru->texture(mi.tex);
                        bru->render(vp, vec4(dp.x + leftSize, dp.y + topSize, size.x, size.y));
                    }
                }
            }

            if (_textureName.empty())
                return;

            string texName = _textureName;
            if (skin *s = getSkin())
                texName = s->getTexture(texName);

            Ogre::TexturePtr tex = gfx::getTexture(texName);
            if (!tex.get())
            {
                _textureName.clear();
                return;
            }

//            if (!b)
//            {
//                _textureName.clear();
//                log("widget " + _name + " has no brush");
//            }

            vec4 dcr = derivedChildrenRect();

            bru->texture(texName);

            if (clickable && _mouseDown)
                bru->render(vp, dcr, brush::E_PRESS);
            else
            {
                if (blend)
                    bru->render(vp, dcr);
                else
                    bru->render(vp, dcr, brush::E_NO_BLEND);

                // if the mouse is over us and we're not fading
                if (clickable && _mouseOver && derivedVisible())
                {
                    bru->render(vp, dcr, brush::E_SELECT);
                }
            }
        }

        vec2 panel::skinOffset(const align_t &a) const
        {
            if (decorate)
            {
                if (skin *s = getSkin())
                {
                    const string TYPE = getSkinType();

                    const skin::piece &tl = s->getPiece(TYPE, A_TL);
                    const skin::piece &tr = s->getPiece(TYPE, A_TR);
                    const skin::piece &t = s->getPiece(TYPE, A_T);

                    float topSize = std::max(std::max(tl.size.y, tr.size.y), t.size.y);

                    const skin::piece &bl = s->getPiece(TYPE, A_BL);
                    const skin::piece &br = s->getPiece(TYPE, A_BR);
                    const skin::piece &b = s->getPiece(TYPE, A_B);

                    const skin::piece &l = s->getPiece(TYPE, A_L);
                    const skin::piece &r = s->getPiece(TYPE, A_R);

                    float leftSize = std::max(std::max(tl.size.x, bl.size.x), l.size.x);

                    float rightSize = std::max(std::max(tr.size.x, br.size.x), r.size.x);

                    float bottomSize = std::max(std::max(bl.size.y, br.size.y), b.size.y);

//                    const skin::piece &p = s->getPiece(getSkinType(), a);
//                    return p.size;
                    if (a == A_TL) return vec2(leftSize, topSize);
                    else if (a == A_T) return vec2(0, topSize);
                    else if (a == A_TR) return vec2(rightSize, topSize);
                    else if (a == A_R) return vec2(rightSize, 0);
                    else if (a == A_BR) return vec2(rightSize, bottomSize);
                    else if (a == A_B) return vec2(0, bottomSize);
                    else if (a == A_BL) return vec2(leftSize, bottomSize);
                    else if (a == A_L) return vec2(leftSize, 0);
                    else if (a == A_MI) return vec2(0, 0);
                }
            }
            return vec2(0, 0);
        }

        boost::any panel::attrib(const string &name) const
        {
            if (name == "texture" || name == "textureName") return _textureName;
            else return widget::attrib(name);
            return boost::any();
        }

        widget &panel::update(const string &name, const boost::any &val)
        {
            //using boost::any_cast;
            if (name == "texture" || name == "textureName") _textureName = SAFE_ANY_CAST(string, val);
            else if (name == "autoSize" && !_textureName.empty())
            {
                string texName = _textureName;
                if (skin *s = getSkin())
                    texName = s->getTexture(texName);

                Ogre::TexturePtr tex = gfx::getTexture(texName);
                if (tex.get())
                {
                    update
                        ("size", vec2(tex->getWidth(), tex->getHeight()));
                }
            }
            else return widget::update(name, val);

            return *this;
        }
    }
}
