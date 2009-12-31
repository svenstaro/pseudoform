 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef LAYOUT_HPP_INCLUDED
#define LAYOUT_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

//#include "widget.hpp"
#include "types.hpp"

namespace engine
{
    namespace gui
    {

        class widget;

        class layoutItem
        {
            public:
                virtual ~layoutItem() {}

                virtual vec2 size() const = 0;
                virtual void size(const vec2 &v) = 0;

                virtual vec2 pos() const = 0;
                virtual void pos(const vec2 &v) = 0;

                virtual void tick(real dt) {}
        };

        typedef boost::shared_ptr<layoutItem> layoutItemPtr;

        typedef std::vector<layoutItemPtr> layoutItemList;

        class widgetLayoutItem: public layoutItem
        {
            private:

                boost::shared_ptr<widget> _widget;

            public:

                widgetLayoutItem(const boost::shared_ptr<widget> &w);

                vec2 size() const;
                void size(const vec2 &v);

                vec2 pos() const;
                void pos(const vec2 &v);
        };

        class layout;

        typedef boost::shared_ptr<layout> layoutPtr;

        class layout
        {
            protected:

                layoutItemList _items;

            public:

                const layoutItemList::size_type numItems() const
                {
                    return _items.size();
                }

                float padding;

                layout(float pad = 4);
                virtual ~layout();

                void addWidget(const boost::shared_ptr<widget> &w);
                void addLayout(const layoutPtr &lay);

                vec2 size() const;

                virtual void tick(real dt) = 0;
        };

        class layoutLayoutItem: public layoutItem
        {
            private:

                layoutPtr _layout;

            public:

                layoutLayoutItem(const layoutPtr &lay);

                vec2 size() const;
                void size(const vec2 &v);

                vec2 pos() const;
                void pos(const vec2 &v);

                void tick(real dt);
        };

        class verticalLayout: public layout
        {
            public:

                void tick(real dt);
        };

        class horizontalLayout: public layout
        {
            public:

                void tick(real dt);
        };
    }
}

#endif // LAYOUT_HPP_INCLUDED
