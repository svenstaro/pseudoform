 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SCROLLABLE_PANEL_HPP_INCLUDED
#define SCROLLABLE_PANEL_HPP_INCLUDED

#include "panel.hpp"
#include "scrollbar.hpp"

namespace engine
{


    namespace gui
    {

        class scrollablePanel: public panel
        {
            protected:

                scrollbarPtr _xScrollbar, _yScrollbar;
                vec4 _childrenRect;
                vec2 _childrenOffset;
                vec2 _smoothOffset;

                void _calcChildrenRect();
                void _updateScrollbars();
                void _limitChildrenOffset();

                void _scroll(const event &e);

            public:

                virtual vec2 childrenOffset() const
                {
//                    _calcChildrenRect();
//                    _limitChildrenOffset();
//                    return -_childrenOffset; // this is added, but we need it to be subtracted
                    return panel::childrenOffset() + -_smoothOffset;
                }

                scrollablePanel(const string &name, bool internal = false);
                virtual ~scrollablePanel();

                virtual void tick(real dt);

//                virtual void scroll(const vec2 &v);

        };

        typedef boost::shared_ptr<scrollablePanel> scrollablePanelPtr;

    }


}

#endif // SCROLLABLE_PANEL_HPP_INCLUDED
