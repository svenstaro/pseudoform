 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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
		/**
		 * Represents mouse object
		 */
        class mouse;

        /**
         * Shared pointer for the mouse object
         */
        typedef boost::shared_ptr<mouse> mousePtr;

        /**
         * Represents text in 3d-game
         */
        class text;

        /**
         * \brief Default gui unit
         *
         * All widgets are based on this class
         */
        class sheet: public widget
        {
            private:
				/// Mouse object
                mousePtr _mouse;

                /// Flying tip for current widget
                boost::shared_ptr<text> _tip;

                /// TODO
                widgetPtr _widgetUnderMouse, _draggingWidget, _activeWidget, _clickedWidget;

                // Is current widget dragging TODO: Check
                bool _draggedWidget;

                /// Last time widget was clicked. Used for denying fast clicks.
                real _timeSinceClick;

                /**
                 * Updates widget that is under mouse cursor TODO: check
                 */
                void _updateWidgetUnderMouse();

            public:

                /**
                 * Get mouse position
                 *
                 * @return vector2 mouse vector position
                 */
                vec2 mousePos() const;

                /**
                 * \brief Get normalized mouse position
                 *
                 * Normalize in [0..1]
                 * @return normalized vector2 of mouse position
                 */
                vec2 mousePos01() const;

                /**
                 * Constructor
                 */
                sheet(const string &name);

                /**
                 * Destructor
                 */
                ~sheet();

                // Some basic events for the mouse

                /**
                 * Call mouse move event
                 * @param v vector2 mouse move direction
                 */
                void mouseMove(vec2 v);

                /**
                 * Call mouse scrool event
                 * @param v degrees to scroll
                 */
                void mouseScroll(float v);

                /**
                 * Call mouse up event
                 * @param b mouse button type to generate event for
                 */
                void mouseUp(mouseButton b);

                /**
                 * Call mouse down event
                 * @param b mouse button type to generate event for
                 */
                void mouseDown(mouseButton b);

                /**
                 * Call key down event
                 * @param k key code of pushed button
                 */
                void keyDown(unsigned k);

                /**
                 * Call key down event
                 * @param k TODO
                 */
                void keyDown(specialKey k);

                /**
                 * Get pointer to the mouse object
                 */
                mousePtr mouse() const
                {
                    return _mouse;
                }

                /**
                 * Get widget which is located under mouse
                 * @return pointer to the fount widget
                 */
                widgetPtr widgetUnderMouse() const;

                /**
                 * Get active widget
                 * @return pointer to the active widget
                 */
                widgetPtr activeWidget() const { return _activeWidget; }

                /**
                 * Make current widget active
                 */
                bool active() const;

                /**
                 * Render current widget
                 */
                void render(Ogre::Viewport *vp);

                /**
                 * Update widget from global application cycle
                 */
                void tick(real dt);

                /**
                 * Update widget with given name
                 * @param name widget to get pointer for
                 * @param val TODO
                 *
                 * @return link for the updated widget
                 */
                widget &update(const string &name, const boost::any &val = boost::any())
                {
                    if (name == "drag" || name == "move")
                    {
                        // don't drag/move sheets
                    }
                    else
                        return widget::update(name, val);
                    return *this;
                }
        };

        /**
         * Shared pointer to the current class
         */
        typedef boost::shared_ptr<sheet> sheetPtr;
    }
}

#endif // SHEET_HPP_INCLUDED
