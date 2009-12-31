 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <cmath>
#include <numeric>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>

#include "scrollable_panel.hpp"
#include "safe_any.hpp"

namespace engine
{


    namespace gui
    {

        scrollablePanel::scrollablePanel(const string &name, bool internal):
            panel(name, internal),
            _xScrollbar(new scrollbar("xScrollbar", true)),
            _yScrollbar(new scrollbar("yScrollbar", true)),
            _childrenRect(0, 0, 1, 1),
            _childrenOffset(0, 0),
            _smoothOffset(0, 0)
        {
            _interfaces.push_back(WIDGET_SCROLLABLE_PANEL);

            addChild(_xScrollbar);
            _xScrollbar->visible = false;
            _xScrollbar->obeyOffset = false;
            _xScrollbar->clippable = false;
            _xScrollbar->decorate = true;
            _xScrollbar->setCallback("onScroll", boost::bind(&scrollablePanel::_scroll, this, _1));

            addChild(_yScrollbar);
            _yScrollbar->visible = false;
            _yScrollbar->obeyOffset = false;
            _yScrollbar->clippable = false;
            _yScrollbar->decorate = true;
            _yScrollbar->setCallback("onScroll", boost::bind(&scrollablePanel::_scroll, this, _1));
        }

        scrollablePanel::~scrollablePanel()
        {
        }

        void scrollablePanel::_scroll(const event &e)
        {
            bool vert = (&e.caller == _yScrollbar.get());

            float a = (vert ? _childrenRect.y : _childrenRect.x);
            float b = (vert ? _childrenRect.w - size.y : _childrenRect.z - size.x);

            if (b < a)
                b = a; // make sure right/bottom >= left/top
            float f = SAFE_ANY_CAST(float, e.misc);

            if (vert)
                _childrenOffset.y = a + (b - a) * f;
            else
                _childrenOffset.x = a + (b - a) * f;
        }

        void scrollablePanel::tick(real dt)
        {
            _calcChildrenRect();
            _limitChildrenOffset();
            _updateScrollbars();

            const float SMOOTH_SPEED = 8;
            _smoothOffset += (_childrenOffset - _smoothOffset) * dt * SMOOTH_SPEED;

            panel::tick(dt);
        }

        void scrollablePanel::_limitChildrenOffset()
        {
            // limit left
            if (_childrenOffset.x < _childrenRect.x)
                _childrenOffset.x = _childrenRect.x;
            // limit top
            if (_childrenOffset.y < _childrenRect.y)
                _childrenOffset.y = _childrenRect.y;

            // limit right (only up to right - width of scrollablePanel)
            if (_childrenOffset.x > _childrenRect.z - size.x)
                _childrenOffset.x = _childrenRect.z - size.x;

            // limit bottom (only up to bottom - height of scrollablePanel)
            if (_childrenOffset.y > _childrenRect.w - size.y)
                _childrenOffset.y = _childrenRect.w - size.y;
        }

        void scrollablePanel::_updateScrollbars()
        {
            _xScrollbar->visible = _childrenRect.x < 0 || _childrenRect.z > size.x;
            _xScrollbar->percent(_xScrollbar->percent());

            _yScrollbar->visible = _childrenRect.y < 0 || _childrenRect.w > size.y;
            _yScrollbar->percent(_yScrollbar->percent());

            /*
                we originally get the children offset from the scrollbar as so:

                a + (b - a)x = c

                when we break it down to get the reverse (percent), we get:

                x(b - a) + a = c
                x(b - a) = c - a
                x        = c - a
                           -----
                           b - a

                c = _childrenOffset
                x = percent
                a = _childrenRect.xy
                b = _childrenRect.zw - size.xy

            */
            float x = (_childrenOffset.x - _childrenRect.x) / (_childrenRect.z - size.x - _childrenRect.x);
            float y = (_childrenOffset.y - _childrenRect.y) / (_childrenRect.w - size.y - _childrenRect.y);

            _xScrollbar->percent(x);
            _yScrollbar->percent(y);

            vec4 r = rect();
            vec2 s = _xScrollbar->getDragger()->size; // assume _yScrollbar has same size
            _xScrollbar->points(vec2(0, r.w), vec2(size.x - s.x, r.w));

            //! iffy issue here because windows usually don't have a A_T skin part
            //! so we get an ugly offset that doesn't match up >.<
            //! so we need to account for this crap :(
            float offset = _yScrollbar->skinOffset(A_T).y - skinOffset(A_T).y;
            _yScrollbar->points(vec2(r.z, 0), vec2(r.z, size.y - s.y - offset));
        }

        void scrollablePanel::_calcChildrenRect()
        {
            _childrenRect = vec4(0, 0, size.x, size.y);
            if (_children.empty())
                return;

            BOOST_FOREACH(const widgetMap::value_type &i, _children)
            {
                if (!i.second->obeyOffset)
                    continue;

                vec4 r = i.second->rect();

                vec2 p(r.x, r.y), s(r.z, r.w);

                _childrenRect.x = std::min(p.x, _childrenRect.x);
                _childrenRect.y = std::min(p.y, _childrenRect.y);
                _childrenRect.z = std::max(p.x + s.x, _childrenRect.z);
                _childrenRect.w = std::max(p.y + s.y, _childrenRect.w);
            }
        }

//        void scrollablePanel::scroll(const vec2 &v)
//        {
//            // simulate dragging
//            if (_xScrollbar->visible) _xScrollbar->update("drag", vec2(v.x, 0));
//            if (_yScrollbar->visible) _yScrollbar->update("drag", vec2(0, v.y));
//        }

    }

}

