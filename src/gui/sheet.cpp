 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

/*
 * Update list
 * comments
 * date
 */

#include <boost/foreach.hpp>

#include <OgreRoot.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreSceneManager.h>
#include <OgreViewport.h>
#include <OgreCamera.h>
#include <OgreRenderSystem.h>

#include "sheet.hpp"
#include "mouse.hpp"
#include "text.hpp"

namespace engine
{
    namespace gui
    {

		/// Offset of flying tip from parent widget
        const vec2 TIP_OFFSET(32, 0);

        sheet::sheet(const string &name):
            widget(name),
            _mouse(new ::engine::gui::mouse("mouse")),
            _tip(new text("tip")),
            _draggedWidget(false),
            _timeSinceClick(10)
        {
            child(_tip)
                ("decorate", true)
                ("visible", false);

            // Don't make the mouse a child or _widgetUnderMouse
        }

        sheet::~sheet()
        {
        }

        void sheet::mouseMove(vec2 v)
        {
            vec2 dp = _mouse->derivedPosition();
            if (dp.x + v.x < 0 || dp.x + v.x > size.x)
                v.x = 0;
            if (dp.y + v.y < 0 || dp.y + v.y > size.y)
                v.y = 0;
            if (_mouse->visible && _draggingWidget && _draggingWidget->draggable)
            {
                vec2 p = _draggingWidget->derivedPosition();

                _draggingWidget->drag(v);
                _draggedWidget = true;

                // Dragging, move only as much as dragging allows
                _mouse->move(_draggingWidget->derivedPosition() - p);
            }
            else if (_mouse->visible)
            {
                // Not dragging, just move
                _mouse->move(v);
                if (_tip->visible)
                {
                    _tip->update
                        ("position", _mouse->position + TIP_OFFSET);
                }
            }

            _updateWidgetUnderMouse();
        }

        void sheet::mouseScroll(float v)
        {
        	// TODO: mouse scroll event
//            if (_widgetUnderMouse)
//            {
//                widget *p = _widgetUnderMouse;
//                while (p && !p->scrollable())
//                    p = p->parent();
//
//                if (p) p->scroll(vec2(0, v));
//            }
        }

        void sheet::mouseUp(mouseButton b)
        {
            if (_widgetUnderMouse && !_draggedWidget)
            {
                _widgetUnderMouse->mouseUp(_mouse->derivedPosition(), b);
            }

            // Update active widget
            {
                widgetPtr old = _activeWidget;
                if (_widgetUnderMouse)
                    _activeWidget = _widgetUnderMouse;
                else
                    _activeWidget.reset();

                if (old != _activeWidget)
                {
                    if (old)
                        old->active = false;

                    if (_activeWidget)
                        _activeWidget->active = true;
                }
            }

            if (b == MB_LEFT)
            {
                _draggingWidget.reset();
                _draggedWidget = false;
            }
        }

        void sheet::mouseDown(mouseButton b)
        {
            bool dClicked = false;
            if (b == MB_LEFT)
            {
                _draggingWidget = _widgetUnderMouse;
                _draggedWidget = false;

                // Activate widget (bring to front)
                if (_widgetUnderMouse)
                {
                    _widgetUnderMouse->bringToFront();

                    // We want to double click only if it's the same widget and under 0.5 seconds
                    if (_timeSinceClick < 0.5 && _clickedWidget == _widgetUnderMouse)
                    {
                        _widgetUnderMouse->mouseDoubleClick(_mouse->derivedPosition());
                        // we don't want to execute mouseDown() if this was a double-click
                        dClicked = true;
                        _timeSinceClick = 10;
                        _clickedWidget.reset();
                    }
                    else
                    {
                        _timeSinceClick = 0;
                        _clickedWidget = _widgetUnderMouse;
                    }
                }
                else
                {
                    _timeSinceClick = 10;
                    _clickedWidget.reset();
                }
            }

            if (_widgetUnderMouse && !dClicked)
            {
                _widgetUnderMouse->mouseDown(_mouse->derivedPosition(), b);
            }
        }

        void sheet::keyDown(unsigned k)
        {
            if (_activeWidget)
                _activeWidget->keyDown(k);
        }


        void sheet::keyDown(specialKey k)
        {
            if (_activeWidget)
                _activeWidget->keyDown(k); // Redirect event
        }

        vec2 sheet::mousePos01() const
        {
            return _mouse->position / size;
        }

        vec2 sheet::mousePos() const
        {
            return _mouse->position;
        }

        void sheet::_updateWidgetUnderMouse()
        {
            widgetPtr old = _widgetUnderMouse;

            if (widgetPtr w = childAtPtr(_mouse->position))
            {
                widget *temp = w->select();
                if (temp != w.get())
                    // We need to retrive a widgetPtr
                    _widgetUnderMouse = temp->smartPtr();
                else
                    _widgetUnderMouse = w;
            }
            else
            {
                _widgetUnderMouse.reset();
            }

            if (old != _widgetUnderMouse)
            {
                _tip->update
                    ("visible", false);

                if (old)
                    old->mouseOver(false);

                if (_widgetUnderMouse)
                {
                    _widgetUnderMouse->mouseOver(true);
                    if (!_widgetUnderMouse->tip.empty())
                    {
                        _tip->update
                            ("text", _widgetUnderMouse->tip)
                            ("visible", true)
                            ("position", _mouse->position + TIP_OFFSET);
                    }
                }
            }
        }

        widgetPtr sheet::widgetUnderMouse() const
        {
            return _widgetUnderMouse;
        }

        bool sheet::active() const
        {
            return (_mouse->visible && _widgetUnderMouse) || _draggingWidget;
        }

        void sheet::render(Ogre::Viewport *vp)
        {
            // Sheets need to only render the children
            _renderChildren(vp);

            // Get rid of clipping for mouse/scene
            _clip(false, vec4());

            // Mouse is always last
            _mouse->render(vp);
        }

        void sheet::tick(real dt)
        {
            _timeSinceClick += dt;
            widget::tick(dt);
            _mouse->tick(dt);
        }
    }
}
