 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <boost/foreach.hpp>

#include <OgreSceneManager.h>
#include <OgreRenderTexture.h>
#include <OgreRenderSystem.h>
#include <OgreTextureManager.h>
#include <OgreTexture.h>
#include <OgreCamera.h>
#include <OgreMaterialManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreRoot.h>

#include "widget.hpp"
#include "log.hpp"
#include "no_impl.hpp"
#include "safe_any.hpp"
#include "brush.hpp"
#include "skin.hpp"
#include "layout.hpp"

namespace engine
{
    namespace gui
    {
        dummyWidget widget::_dummyWidget;

        brush *widget::_brush = NULL;
        int widget::_numWidgets = 0;

        idGen widget::_idGen;

        brush *widget::getBrush()
        {
            assert(_brush && "must create widget before using widget::getBrush");
            return _brush;
        }

        // dummy constructor
        widget::widget():
            _name("widget::_dummyWidget"),
            _uniqueName(_name),
            _parent(NULL),
            _internal(false),
            _colour(1, 1, 1, 1),
            _alpha(0),
            _mouseOver(false),
            _mouseDown(false),
            position(0, 0),
            size(1, 1),
            visible(false),
            draggable(false),
            obeyOffset(false),
            clippable(true),
            active(false),
            decorate(false),
            clickable(true),
            blend(true),
            selectParent(false)
        {
            ++_numWidgets;
        }

        widget::widget(const string &name, bool internal):
            _name(name),
            _uniqueName(_idGen(_name)),
            _parent(NULL),
            _internal(internal),
            _colour(1, 1, 1, 1),
            _alpha(0),
            _mouseOver(false),
            _mouseDown(false),
            position(0, 0),
            size(1, 1),
            visible(true),
            draggable(true),
            obeyOffset(true),
            clippable(true),
            active(false),
            decorate(true),
            clickable(true),
            blend(true),
            selectParent(false)
        {
            ++_numWidgets;

            if (!_brush)
            {
                _brush = new brush();
            }
        }

        widget::~widget()
        {
            --_numWidgets;

            if (_numWidgets <= 1) // last widget is always the dummy widget
            {
                delete _brush;
                _brush = NULL;
            }

            _idGen -= _uniqueName;
        }

        widget &widget::child(const string &name)
        {
            widgetPtr ptr = childPtr(name);
            if (!ptr)
            {
                log("cannot find child \"" + name + "\" in parent \"" + _name + "\"");
                return _dummyWidget;
            }
            return *ptr;
        }

        widget &widget::childInFamily(const string &name)
        {
            widgetPtr ptr = childInFamilyPtr(name);
            if (!ptr)
                return _dummyWidget;
            return *ptr;
        }

        widget &widget::child(const widgetPtr &w)
        {
            if (!w)
            {
                log("tried to add NULL child to widget \"" + _name + "\"");
                return _dummyWidget;
            }
            assert(w);

            if (w->_parent)
            {
                log("tried to add widget \"" + w->name() + "\" as child to \"" + _name + "\""
                    " but parent exists \"" + w->_parent->name() + "\"");
                return *w;
            }

            _children[w->name()] = w;
            w->_parent = this;

            if (w->internal())
                _orderedChildren.push_front(w.get());
            else
                _orderedChildren.push_back(w.get());

            return *w;
        }

        void widget::removeChild(const string &name)
        {
            widgetMap::iterator i = _children.find(name);
            if (i != _children.end())
            {
                _orderedChildren.remove(i->second.get());
                i->second->_parent = NULL;
                _children.erase(i);
            }
        }

        void widget::clearChildren(bool clearInternal)
        {
            std::list<widgetMap::iterator> toRemove;

            for (widgetMap::iterator i = _children.begin(); i != _children.end(); ++i)
            {
                if (clearInternal || !i->second->internal())
                {
                    i->second->_parent = NULL;
                    _orderedChildren.remove(i->second.get());
                    toRemove.push_back(i);
                }
            }

            BOOST_FOREACH(widgetMap::iterator &i, toRemove)
                _children.erase(i);
        }

        widgetPtr widget::childInFamilyPtr(const string &name)
        {
            widgetMap::iterator i = _children.find(name);
            if (i != _children.end())
                return i->second;
            for (i = _children.begin(); i != _children.end(); ++i)
            {
                widgetPtr w = i->second->childInFamilyPtr(name);
                if (w)
                    return w;
            }
            return widgetPtr();
        }

        widgetPtr widget::childPtr(const string &name)
        {
            widgetMap::iterator i = _children.find(name);
            if (i != _children.end())
                return i->second;
            return widgetPtr();
        }

        widgetPtr widget::childAtPtr(const vec2 &p) const
        {
            BOOST_FOREACH(const widget *w, _orderedChildren)
            {
                // if this widget contains the point
                if (w->contains(p))
                {
                    // check if any of the children contain it, too
                    if (widgetPtr c = w->childAtPtr(p)) // if so, return
                        return c;
                    // if not, just return this parent
                    // (we'll need to fetch the widgetPtr from the map)
                    return w->smartPtr();
                }
            }
            return widgetPtr();
        }

        void widget::bringChildToFront(widget &c)
        {
            _orderedChildren.remove(&c);

            if (c.internal())
                _orderedChildren.push_back(&c);
            else
            {
                widgetList::iterator i = _orderedChildren.begin();
                for (; (*i)->internal() && i != _orderedChildren.end(); ++i)
                    // find the next non-internal child
                    ;
                if (i == _orderedChildren.end())
                    _orderedChildren.push_back(&c);
                else
                    _orderedChildren.insert(++i, &c);
            }
        }

        vec2 widget::derivedPosition() const
        {
            if (!_parent)
                return position;
            if (obeyOffset)
                return _parent->childrenPosition() + position + _parent->childrenOffset();
            return _parent->derivedPosition() + position;
        }

        bool widget::within(const vec4 &v) const
        {
//            vec2 p = derivedPosition();
//            return p + size >= vec2(r.x, r.y) && p <= vec2(r.z, r.w);
            vec4 r = derivedRect();
            return vec2(r.x + r.z, r.y + r.w) > vec2(v.x, v.y) && vec2(r.x, r.y) < vec2(v.z, v.w);
        }

        vec4 widget::rect() const
        {
            vec2 top = skinOffset(A_TL), bottom = skinOffset(A_BR);
            return vec4(position.x, position.y, size.x + top.x + bottom.x, size.y + top.y + bottom.y);
        }

        vec4 widget::derivedRect() const
        {
            vec2 p = derivedPosition();
            vec2 top = skinOffset(A_TL), bottom = skinOffset(A_BR);
            return vec4(p.x, p.y, size.x + top.x + bottom.x, size.y + top.y + bottom.y);
        }

        void widget::sizeWithSkin(const vec2 &v)
        {
            size = v - skinOffset(A_TL) - skinOffset(A_BR);
        }

        bool widget::contains(const vec2 &p) const
        {
            if (!visible)
                return false;
//
//            vec2 dp = derivedPosition();
//            if (p >= dp && p <= dp + size)
//                return true;
            vec4 r = derivedRect();
            if (p.x >= r.x && p.y >= r.y && p.x <= r.x + r.z && p.y <= r.y + r.w)
                return true;

            // also check the children, which may be not clippable
            // and thus outside our own region
            BOOST_FOREACH(const widgetMap::value_type &i, _children)
                if (i.second->visible && !i.second->clippable && i.second->contains(p))
                    return true;

            return false;
        }

        void widget::render(Ogre::Viewport *vp)
        {
            if (_alpha < 0.001 && !visible)
                return;

            _renderThis(vp);
            _renderChildren(vp);
        }

        void widget::_renderThis(Ogre::Viewport *vp)
        {
        }

        void widget::_renderChildren(Ogre::Viewport *vp)
        {
            // our clip rect and our parent's
            vec4 ourClip = clipRect(), parentClip = (_parent ? _parent->clipRect() : ourClip);

//            BOOST_FOREACH(widget *p, _orderedChildren)
            for (widgetList::reverse_iterator i = _orderedChildren.rbegin(); i != _orderedChildren.rend(); ++i)
            {
                widget *p = *i;

                vec4 clipper = (p->clippable ? ourClip : parentClip);
                if (p->within(clipper))
                {
                    // (re)set clipping for current child - if it wants to be clipped
                    // then we use our clip rect, otherwise our parent's
                    _clip(true, clipper);
                    p->render(vp);
                }
            }
        }

        void widget::tick(real dt)
        {
            const float ALPHA_SPEED = 8;
            if (!visible)
            {
                _alpha = std::max<float>(_alpha - dt * ALPHA_SPEED, 0);
            }
            else
            {
                _alpha = std::min<float>(_alpha + dt * ALPHA_SPEED, 1);
            }

            if (_layout)
                _layout->tick(dt);

            // our clip rect and our parent's
            vec4 ourClip = clipRect(), parentClip = (_parent ? _parent->clipRect() : ourClip);

            // not much to do, tell children
            BOOST_FOREACH(const widgetMap::value_type &i, _children)
            {
                vec4 clipper = (i.second->clippable ? ourClip : parentClip);
                if (i.second->within(clipper))
                    i.second->tick(dt);
            }
        }

        void widget::mouseOver(bool b)
        {
//            if (b) _colourMult = _mouseOverColour;
//            else _colourMult = colour(1, 1, 1, 1);
            _mouseOver = b;
            if (!_mouseOver)
                _mouseDown = false; // can't be down if not over
        }

        void widget::mouseDown(const vec2 &p, mouseButton b)
        {
//            if (b == MB_LEFT)
//                _colourMult = _mouseDownColour;
            _fireCallback("onMouseDown", b);
            if (b == MB_LEFT)
                _mouseDown = true;
        }

        void widget::mouseUp(const vec2 &p, mouseButton b)
        {
            if (b == MB_LEFT)
            {
                _fireCallback("onClick", b);
                _mouseDown = false;
//                _colourMult = _mouseOverColour;
            }
            _fireCallback("onMouseUp", b);
        }

        void widget::mouseDoubleClick(const vec2 &p)
        {
            _fireCallback("onDoubleClick", p);
        }

        void widget::_clip(bool b, const vec4 &v) const
        {
            Ogre::Root::getSingleton().getRenderSystem()->setScissorTest(
                b,
                size_t(v.x), size_t(v.y), size_t(v.z), size_t(v.w));
        }

        boost::any widget::attrib(const string &name) const
        {
            if (name == "misc") return misc;

            else if (name == "decorate") return decorate;

            else if (name == "clickable") return clickable;

            else if (name == "colour") return _colour;

            else if (name == "tip") return tip;

            else if (name == "selectParent") return selectParent;

            else
            {
                log("widget attrib \"" + name + "\" not found in widget \"" + _name + "\"");
                return boost::any();
            }

            return boost::any();
        }

        widget &widget::update(const string &name, const boost::any &val)
        {
            //using boost::any_cast;

            if (name == "position") position = SAFE_ANY_CAST_T(vec2, val, vec2(0, 0));

            else if (name == "size") size = SAFE_ANY_CAST_T(vec2, val, vec2(128, 128));

            else if (name == "visible") visible = SAFE_ANY_CAST_T(bool, val, true);

            else if (name == "draggable") draggable = SAFE_ANY_CAST_T(bool, val, false);

            else if (name == "obeyOffset") obeyOffset = SAFE_ANY_CAST_T(bool, val, true);

            else if (name == "clippable") clippable = SAFE_ANY_CAST_T(bool, val, true);

            else if (name == "colour") _colour = SAFE_ANY_CAST_T(colour, val, colour(1, 1, 1, 1));

            else if (name == "decorate") decorate = SAFE_ANY_CAST_T(bool, val, true);

            else if (name == "clickable") clickable = SAFE_ANY_CAST_T(bool, val, true);

            else if (name == "blend") blend = SAFE_ANY_CAST_T(bool, val, true);

            else if (name == "tip") tip = SAFE_ANY_CAST(string, val);

            else if (name == "selectParent") selectParent = SAFE_ANY_CAST_T(bool, val, false);

//            else if (name == "sizeWithSkin") sizeWithSkin(SAFE_ANY_CAST_T(vec2, val, vec2(128, 128)));

//            else if (name == "skin")
//            {
//                string str = SAFE_ANY_CAST_T(string, val, string());
//                if (!str.empty())
//                    setSkin(skinPtr(new skin(str)));
//                else
//                    resetSkin();
//            }

            else if (name == "misc") misc = val;

            else
            {
                log("widget update \"" + name + "\" not found in widget \"" + fullName() + "\"");
                return *this;
            }

            return *this;
        }

        vec4 widget::clipRect() const
        {
            vec2 p = childrenPosition();
            vec4 r(p.x, p.y, p.x + size.x, p.y + size.y);

            /*! this poses a bit of any issue
                the close button on a titlebar won't get clipped
                along with the title bar because the title bar itself
                is not clippable ... how to fix?
                */
            widget *clipper = _parent;
            if (!clippable && clipper)
                clipper = clipper->_parent;

            if (clipper)
            {
                vec4 r2 = clipper->clipRect();
                if (r.x < r2.x) r.x = r2.x;
                if (r.y < r2.y) r.y = r2.y;
                if (r.z > r2.z) r.z = r2.z;
                if (r.w > r2.w) r.w = r2.w;
            }

            return r;
        }

        void resizeToFitChildren(widget &w, const vec2 &maxSize)
        {
            vec2 size(0, 0);
            BOOST_FOREACH(const widgetMap::value_type &i, w.children())
            {
                size.x = std::max(size.x, i.second->size.x + i.second->position.x);
                size.y = std::max(size.y, i.second->size.y + i.second->position.y);
            }
            size.x = std::min(maxSize.x, size.x);
            size.y = std::min(maxSize.y, size.y);

            w.update
                ("size", size);
        }
    }
}
