 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SHEET_HPP_INCLUDED
#define SHEET_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include "pre_ogre.hpp"
#include "string.hpp"
#include "id.hpp"
#include "widget.hpp"
#include "brush.hpp"
#include "mouse_button.hpp"
//#include "panel.hpp"

namespace engine
{

    namespace gui
    {

        class mouse;
        typedef boost::shared_ptr<mouse> mousePtr;

        class text;

        class sheet: public widget
        {
            private:

                mousePtr _mouse;

                boost::shared_ptr<text> _tip;

                widgetPtr _widgetUnderMouse, _draggingWidget, _activeWidget, _clickedWidget;
                bool _draggedWidget;

                real _timeSinceClick;

                void _updateWidgetUnderMouse();

            public:

                // shortcut
                vec2 mousePos() const;
                vec2 mousePos01() const; // normalize in [0..1]

                sheet(const string &name);
                ~sheet();

                void mouseMove(vec2 v);
                void mouseScroll(float v);
                void mouseUp(mouseButton b);
                void mouseDown(mouseButton b);

                void keyDown(unsigned k);
                void keyDown(specialKey k);

                mousePtr mouse() const
                {
                    return _mouse;
                }

                widgetPtr widgetUnderMouse() const;
                widgetPtr activeWidget() const { return _activeWidget; }
                bool active() const;

                void render(Ogre::Viewport *vp);
                void tick(real dt);
        };

        typedef boost::shared_ptr<sheet> sheetPtr;

    }

}

#endif // SHEET_HPP_INCLUDED
