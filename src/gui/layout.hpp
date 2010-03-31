 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

        /**
         * \brief Layout unit
         *
         * Inner object of the layout
         */
        class layoutItem
        {
            public:
				/**
				 * Destructor
				 */
                virtual ~layoutItem() {}

                /**
                 * Get layout size
                 */

                virtual vec2 size() const = 0;
                /**
                 * Set layout size
                 * @param v layout size
                 */
                virtual void size(const vec2 &v) = 0;

                /**
                 * Get layout position
                 */
                virtual vec2 pos() const = 0;

                /**
                 * Set layout position
                 * @param v new layout position
                 */
                virtual void pos(const vec2 &v) = 0;

                /**
                 * Update layout from global application cycle
                 */
                virtual void tick(real dt) {}
        };

        /// Shared pointer for the layout item
        typedef boost::shared_ptr<layoutItem> layoutItemPtr;

        /// List of items for layout
        typedef std::vector<layoutItemPtr> layoutItemList;

        /**
         * Widget-based layout item object
         */
        class widgetLayoutItem: public layoutItem
        {
            private:
				/// Item will use this widget
                boost::shared_ptr<widget> _widget;

            public:
                /**
                 * Constructor
                 */
                widgetLayoutItem(const boost::shared_ptr<widget> &w);

                vec2 size() const;
                void size(const vec2 &v);

                vec2 pos() const;
                void pos(const vec2 &v);
        };

        /**
         * Layout system
         */
        class layout;

        /// Shared pointer for the layout system
        typedef boost::shared_ptr<layout> layoutPtr;

        class layout
        {
            protected:
				/// List of items in layout object
                layoutItemList _items;

            public:
				/**
				 * Get number of items in layout
				 */
                const layoutItemList::size_type numItems() const
                {
                    return _items.size();
                }

                /// Layout-items padding from parent border
                float padding;

                /**
                 * Constructor
                 * @param pad default items padding
                 */
                layout(float pad = 4);

                /**
                 * Destructor
                 */
                virtual ~layout();

                /**
                 * Add own widget into the layout
                 * @param w shared pointer for the widget to add in
                 */
                void addWidget(const boost::shared_ptr<widget> &w);

                /**
                 * Add another layout into the layout
                 * @param lay link to the pointer of another layout
                 */
                void addLayout(const layoutPtr &lay);

                vec2 size() const;

                virtual void tick(real dt) = 0;
        };

        // TODO
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

        /**
         * \breif Layout with vertical-stacked items
         */
        class verticalLayout: public layout
        {
			public:
                void tick(real dt);
        };

        /**
         * \breif Layout with horizontal-stacked items
         */
        class horizontalLayout: public layout
        {
            public:

                void tick(real dt);
        };
    }
}

#endif // LAYOUT_HPP_INCLUDED
