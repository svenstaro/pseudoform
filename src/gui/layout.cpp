 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "layout.hpp"
#include "widget.hpp"

namespace engine
{
    namespace gui
    {
        widgetLayoutItem::widgetLayoutItem(const widgetPtr &w):
            _widget(w)
        {
            assert(_widget);
        }

        vec2 widgetLayoutItem::size() const
        {
            vec4 r = _widget->rect();
            return vec2(r.z, r.w);
        }

        void widgetLayoutItem::size(const vec2 &v)
        {
        	// Apply new size
            _widget->sizeWithSkin(v);
        }

        vec2 widgetLayoutItem::pos() const
        {
            return _widget->position;
        }

        void widgetLayoutItem::pos(const vec2 &v)
        {
            _widget->position = v;
        }

        layout::layout(float pad):
            padding(pad)
        {
        }

        layout::~layout()
        {
        }

        vec2 layout::size() const
        {
        	// Go through all items in the layour
        	// and calculate their joined size
            vec2 ret(0, 0);
            BOOST_FOREACH(const layoutItemPtr &i, _items)
            {
                vec2 r = i->pos() + i->size();
                ret.x = std::max<float>(ret.x, r.x);
                ret.y = std::max<float>(ret.y, r.y);
            }
            return ret;
        }

        void layout::addWidget(const boost::shared_ptr<widget> &w)
        {
            if (!w)
            {
                //log("Tried to add null widget to layout");
                //log("Can't add empty widget into the layout");
                return;
            }
            _items.push_back(layoutItemPtr(new widgetLayoutItem(w)));
        }

        void layout::addLayout(const layoutPtr &lay)
        {
            _items.push_back(layoutItemPtr(new layoutLayoutItem(lay)));
        }

        layoutLayoutItem::layoutLayoutItem(const layoutPtr &lay):
            _layout(lay)
        {
            assert(_layout);
        }

        vec2 layoutLayoutItem::size() const
        {
            return _layout->size();
        }

        void layoutLayoutItem::size(const vec2 &v)
        {
        }

        vec2 layoutLayoutItem::pos() const
        {
            return vec2(0, 0);
        }

        void layoutLayoutItem::pos(const vec2 &v)
        {
        }

        void layoutLayoutItem::tick(real dt)
        {
            _layout->tick(dt);
        }

        void verticalLayout::tick(real dt)
        {
            vec2 p(padding, padding);

            BOOST_FOREACH(layoutItemPtr &i, _items)
            {
                i->pos(p);

                // Set new vertical pozition for items
                p.y += i->size().y + padding;
            }
        }

        void horizontalLayout::tick(real dt)
        {
            vec2 p(padding, padding);

            BOOST_FOREACH(layoutItemPtr &i, _items)
            {
                i->pos(p);

                // Set new horizontal pozition for items
                p.x += i->size().x + padding;
            }
        }
    }
}




