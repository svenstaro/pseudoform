 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/foreach.hpp>

#include <OgreRoot.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreSceneManager.h>
#include <OgreViewport.h>
#include <OgreCamera.h>
#include <OgreRenderSystem.h>

//#include <OgreEntity.h>

#include "sheet.hpp"
#include "mouse.hpp"
#include "text.hpp"

namespace engine
{

    namespace gui
    {

        const vec2 TIP_OFFSET(32, 0);

        sheet::sheet(const string &name):
            widget(name),
            _mouse(new ::engine::gui::mouse("mouse")),
            _tip(new text("tip")),
            _draggedWidget(false),
            _timeSinceClick(10)
        {
            _interfaces.push_back(WIDGET_SHEET);
            addChild(_tip);
            _tip->decorate = true;
            _tip->visible = false;
            // don't make the mouse a child or _widgetUnderMouse
            // will always be the mouse x_X
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

                // dragging, move only as much as dragging allows
                _mouse->move(_draggingWidget->derivedPosition() - p);
            }
            else if (_mouse->visible)
            {
                // not dragging, just move
                _mouse->move(v);
                if (_tip->visible)
                {
                    _tip->position = _mouse->position + TIP_OFFSET;
                }
            }

            _updateWidgetUnderMouse();
        }

        void sheet::mouseScroll(float v)
        {
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

            // update active widget
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

                // active widget (bring to front)
                if (_widgetUnderMouse)
                {
                    _widgetUnderMouse->bringToFront();
                    // we want to double click only if it's the same widget and under 0.5 seconds
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
//                std::cout << "clicked on " << _widgetUnderMouse->name() << "\n";
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
                _activeWidget->keyDown(k); // redirect
//            switch(k)
//            {
//                case SK_ESCAPE:
//                    _activeWidget = NULL;
//                break;
//
//                case SK_BACKSPACE:
//                    if (_activeWidget)
//                        _activeWidget->update("backspace");
//                break;
//
//                case SK_ENTER:
//                break;
//
//                case SK_LEFT:
//                    if (_activeWidget)
//                        _activeWidget->update("left");
//                break;
//
//                case SK_UP:
//                    if (_activeWidget)
//                        _activeWidget->update("up");
//                break;
//
//                case SK_RIGHT:
//                    if (_activeWidget)
//                        _activeWidget->update("right");
//                break;
//
//                case SK_DOWN:
//                    if (_activeWidget)
//                        _activeWidget->update("down");
//                break;
//
//                default:
//                break;
//            }
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

            if (widgetPtr w = childAt(_mouse->position))
            {
                widget *temp = w->select();
                if (temp != w.get())
                    // we need to retrive a widgetPtr
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
                _tip->visible = false;

                if (old)
                    old->mouseOver(false);

                if (_widgetUnderMouse)
                {
                    _widgetUnderMouse->mouseOver(true);
                    if (!_widgetUnderMouse->tip.empty())
                    {
                        _tip->setText(_widgetUnderMouse->tip);
                        _tip->visible = true;
                        _tip->position = _mouse->position + TIP_OFFSET;
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
//            _brush.prepareForUse();

            // sheets need to only render the children
            _renderChildren(vp);

            // get rid of clipping for mouse/scene
            _clip(false, vec4());

            // mouse is always last
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
