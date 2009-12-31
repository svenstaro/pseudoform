 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "scrollbar.hpp"
#include "safe_any.hpp"

namespace engine
{

    namespace gui
    {

        dragger::dragger(const string &name, bool internal):
            panel(name, internal),
            _dir(1, 0),
            _len(1),
            _percent(0)
        {
            textureName("scrollbar.png");
            fitTextureSize();
            decorate = false;
        }

        dragger::~dragger()
        {
        }

        void dragger::drag(const vec2 &v)
        {
            position += _dir * _dir.dotProduct(v);

            if (position.squaredLength() > _len * _len)
                position = endPos(); // too far from A, move to B
            else if ((endPos() - position).squaredLength() > _len * _len)
                position = vec2(0, 0); // too far from B, move to A

            _percent = position.length() / _len;

            _fireCallback("onScroll", _percent);
        }

        void dragger::percent(float f)
        {
            _percent = f;
            position = endPos() * f;
        }

        vec2 dragger::endPos() const
        {
            return _parent->size - size;
        }

        void dragger::fixDirAndLen()
        {
            _len = endPos().length();
            if (_len < 0.000001)
                _len = 1;
            _dir = endPos() / _len;
        }

        scrollbar::scrollbar(const string &name, bool i):
            panel(name, i),
            _dragger(new dragger("dragger", true))
        {
            _interfaces.push_back(WIDGET_SCROLLBAR);

            addChild(_dragger);
            _dragger->setCallback("onScroll", boost::bind(&scrollbar::_scroll, this, _1));

            size = vec2(196, 16);
            draggable = false;

            _dragger->fixDirAndLen();
        }

        scrollbar::~scrollbar()
        {
        }

        void scrollbar::_scroll(const event &e)
        {
            _fireCallback("onScroll", e.misc);
        }

        void scrollbar::points(const vec2 &a, const vec2 &b)
        {
            vec2 s = _dragger->size;

            position = a;
            size = b - position + s;

            _dragger->fixDirAndLen();
        }

    }

}
