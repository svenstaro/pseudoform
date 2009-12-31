 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef WIDGET_HPP_INCLUDED
#define WIDGET_HPP_INCLUDED

#include <map>
#include <list>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "pre_ogre.hpp"
#include "mouse_button.hpp"
#include "types.hpp"
#include "id.hpp"

#include "gfx/quad.hpp"
//#include "gfx/rtt.hpp"
#include "skin_pre.hpp"
//#include "layout.hpp"
#include "widget_types.hpp"

namespace engine
{

    namespace gui
    {

        class brush;

        class widget;

        typedef boost::shared_ptr<widget> widgetPtr;
        typedef std::map<string, widgetPtr> widgetMap;
        typedef std::list<widget*> widgetList;
        typedef boost::shared_ptr<widgetList> widgetListPtr;

        struct event
        {
            event(widget &caller, const boost::any &misc):
                caller(caller), misc(misc)
            {
            }

            widget &caller;
            boost::any misc;
        };

        typedef boost::function<void(const event&)> callbackFunc;
        //using boost::bind;
        typedef std::map<string, callbackFunc> callbackMap;

        // quick macro to help with the syntax of creating a widget
        // (reference to
        #define widget_(T, name) (*(new T(name)))

        enum specialKey
        {
            SK_BACKSPACE, SK_ENTER, SK_UP, SK_DOWN, SK_LEFT, SK_RIGHT, SK_ESCAPE
        };

        class skin;
        class layout;

        class widget
        {
            protected:

                static brush *_brush;
                static int _numWidgets;

                interfaceList _interfaces;

                string _name;

                callbackMap _callbacks;
                widgetMap _children;
                widgetList _orderedChildren;

                widget *_parent;

                bool _internal;

                colour _colour;
                float _alpha;

                bool _mouseOver;
                bool _mouseDown;

                boost::shared_ptr<skin> _skin;
                boost::shared_ptr<layout> _layout;

                virtual void _renderThis(Ogre::Viewport *vp);
                virtual void _renderChildren(Ogre::Viewport *vp);

                void _clip(bool b, const vec4 &v) const;

                bool _fireCallback(const string &id, const boost::any &misc)
                {
                    callbackMap::const_iterator i = _callbacks.find(id);
                    if (i == _callbacks.end())
                        return false;
                    else
                        i->second(event(*this, misc));
                    return true;
                }

            public:

                const interfaceList &interfaces() const { return _interfaces; }
                bool hasInterface(const interfaceType &t) const
                {
                    return std::find(_interfaces.begin(), _interfaces.end(), t) != _interfaces.end();
                }
                const interfaceType &type() const
                {
                    assert(!_interfaces.empty());
                    return _interfaces.back();
                }

                static brush *getBrush();

                skinPtr getSkin() const
                {
                    if (_skin)
                        return _skin;
                    if (_parent)
                        return _parent->getSkin();
                    return skinPtr();
                }
                void resetSkin() { _skin.reset(); }
                void setSkin(const boost::shared_ptr<skin> &s) { _skin = s; }

                boost::shared_ptr<layout> getLayout() const { return _layout; }
                void resetLayout() { _skin.reset(); }
                void setLayout(const boost::shared_ptr<layout> &lay) { _layout = lay; }

                bool internal() const { return _internal; }

                const widgetMap &children() const
                {
                    return _children;
                }

                widget *parent() const { return _parent; }

                const string &name() const { return _name; }
                string fullName() const
                {
                    if (_parent)
                        return _parent->fullName() + "::" + _name;
                    return _name;
                }

                widgetPtr smartPtr() const
                {
                    if (!_parent)
                        return widgetPtr();
                    return _parent->child(_name);
                }

                virtual widget *select()
                {
                    return selectParent ? (_parent ? _parent->select() : this) : this;
                }

                widget(const string &name, bool internal = false);
                virtual ~widget();

                // mostly for read purposes
                // write purposes supported, but usage of update()
                // is recommended
                vec2 position;
                vec2 size;
                bool visible;
                bool draggable;
                bool obeyOffset;
                bool clippable;
                bool active;
                bool decorate;
                bool clickable;
                bool blend;
                bool selectParent;
                string tip;

                // user data
                boost::any misc;

                vec4 clipRect() const;

                void bringChildToFront(widget &c);
                void bringToFront()
                {
                    if (_parent)
                    {
                        _parent->bringChildToFront(*this);
                        _parent->bringToFront(); // recurse for parents
                    }
                }

                bool contains(const vec2 &p) const;
                bool within(const vec4 &v) const;

                // derived position
                vec2 derivedPosition() const;
                virtual vec2 skinOffset(const align_t &a = A_TL) const { return vec2(0, 0); }
                virtual string getSkinType() const { return "widget"; }
                vec2 childrenPosition() const { return derivedPosition() + skinOffset(); }

                float alpha() const { return _alpha; }
                float derivedAlpha() const
                {
                    if (_parent)
                        return _parent->derivedAlpha() * _alpha;
                    return _alpha;
                }

                bool derivedVisible() const
                {
                    if (_parent)
                        return _parent->visible && visible;
                    return visible;
                }

                vec4 derivedChildrenRect() const
                {
                    vec2 p = childrenPosition();
                    return vec4(p.x, p.y, size.x, size.y);
                }

                // the following rect() functions give the full rects, including the skin

                vec4 rect() const;
                vec4 derivedRect() const;

                void sizeWithSkin(const vec2 &v);

                // usually for scrolling, etc.
                virtual vec2 childrenOffset() const { return vec2(0, 0); }

                widgetPtr childInFamily(const string &name);
                virtual widgetPtr childAt(const vec2 &p) const;
                widgetPtr child(const string &name);

                void addChild(const widgetPtr &ptr);
                widgetPtr removeChild(const string &name);
                void clearChildren(bool clearInternal = false);

                widget &setCallback(const string &id, const callbackFunc &func)
                {
                    _callbacks[id] = func;
                    return *this;
                }

                void removeCallback(const string &id)
                {
                    callbackMap::iterator i = _callbacks.find(id);
                    if (i != _callbacks.end())
                        _callbacks.erase(i);
                }

                void clearCallbacks()
                {
                    _callbacks.clear();
                }

                virtual void keyDown(specialKey k) { _fireCallback("onSpecialKey", k); }
                virtual void keyDown(unsigned k) {}
                virtual void keyUp(unsigned k) {}

                virtual void scroll(const vec2 &v) {}
                virtual void drag(const vec2 &v)
                {
                    if (draggable)
                    {
                        position += v;
                        _fireCallback("onDrag", v);
                    }
                }
                virtual void move(const vec2 &v)
                {
                    position += v;
                }

                virtual void mouseOver(bool b);
                virtual void mouseUp(const vec2 &p, mouseButton b);
                virtual void mouseDown(const vec2 &p, mouseButton b);
                virtual void mouseDoubleClick(const vec2 &p);

                virtual void render(Ogre::Viewport *vp);
                virtual void tick(real dt);
        };

//        template<typename A>
//        boost::shared_ptr<A> widgetCast(const widgetPtr &w)
//        {
//            if (!w)
//                return boost::shared_ptr<A>();
//            return boost::static_pointer_cast<A>(w);
//        }
//
//        template<typename A>
//        boost::shared_ptr<A> widgetCast(const widgetPtr &w, const interfaceType &type)
//        {
//            if (!w)
//                return boost::shared_ptr<A>();
//            if (!w->hasInterface(type))
//            {
//                log("cannot cast widget " + w->fullName() + " of type " +
//                    boost::lexical_cast<string>(w->type()) + " to type " +
//                    boost::lexical_cast<string>(type));
//                return boost::shared_ptr<A>();
//            }
//            return widgetCast<A>(w);
//        }
//
//        // for conveniency
//        // assumes <type>Ptr naming
//        #define WIDGET_CAST(T, w) widgetCast<T>((w), T::TYPE)

    }

}

#endif // WIDGET_HPP_INCLUDED
