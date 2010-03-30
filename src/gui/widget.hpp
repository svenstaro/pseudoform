 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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
//#include "params.hpp" This is deprecated
#include "gfx/quad.hpp"
//#include "gfx/rtt.hpp"
#include "skin_pre.hpp"
//#include "layout.hpp"

namespace engine
{
    namespace gui
    {
        class brush;

        class widget;

        /// Shared pointer for the widget object
        typedef boost::shared_ptr<widget> widgetPtr;

        /// Widget's map
        typedef std::map<string, widgetPtr> widgetMap;

        /// List of widgets
        typedef std::list<widget*> widgetList;

        /// Shared pointer for the list of widgets
        typedef boost::shared_ptr<widgetList> widgetListPtr;

        /// Widget events base
        struct event
        {
            event(widget &caller, const boost::any &misc):
                caller(caller), misc(misc)
            {
            }

            widget &caller;
            boost::any misc;
        };

        /// Event callback
        typedef boost::function<void(const event&)> callbackFunc;
        typedef std::map<string, callbackFunc> callbackMap;

        // Quick macro to help with the syntax of creating a widget
        // (reference to
        #define widget_(T, name) (*(new T(name)))

        /// List of special keys which can interact with widgets
        enum specialKey
        {
            SK_BACKSPACE, SK_ENTER, SK_UP, SK_DOWN, SK_LEFT, SK_RIGHT, SK_ESCAPE
        };

        class skin;
        class layout;

        class dummyWidget;

        /**
         * Base widget class
         */
        class widget
        {
            private:
                static dummyWidget _dummyWidget;

            protected:
				/// Brush handler for the current widget
                static brush *_brush;
                static int _numWidgets;

                /// List of names for the widgets
                static idGen _idGen;

                /// Widget name
                string _name, _uniqueName;

                /// Map of callback events
                callbackMap _callbacks;

                /// Map of widget's children
                widgetMap _children;

                /// Map of widget's ordered children
                widgetList _orderedChildren;

                /// Pointer to the widget's parent
                widget *_parent;

                // TODO:
                bool _internal;

                colour _colour;
                float _alpha;

                /// Is mouse over widget
                bool _mouseOver;

                /// Is mouse down
                bool _mouseDown;

                /// Shared pointer to the gui skin
                boost::shared_ptr<skin> _skin;

                /// Shared pointer to the gui layouts
                boost::shared_ptr<layout> _layout;

                /**
                 * Render current widget
                 * @param vp viewport to render in
                 */
                virtual void _renderThis(Ogre::Viewport *vp);

                /**
                 * Render all widget's children
                 * @param vp viewport to render in
                 */
                virtual void _renderChildren(Ogre::Viewport *vp);

                void _clip(bool b, const vec4 &v) const;

                inline bool _fireCallback(const string &id, const boost::any &misc)
                {
                    callbackMap::const_iterator i = _callbacks.find(id);
                    if (i == _callbacks.end())
                        return false;
                    else
                        i->second(event(*this, misc));
                    return true;
                }

                /**
                 * Constructor
                 */
                widget();

            public:
                /**
                 * Get brush
                 */
                static brush *getBrush();

                /**
                 * Get widget scin
                 */
                skin *getSkin() const
                {
                    if (_skin)
                        return _skin.get();
                    if (_parent)
                        return _parent->getSkin();
                    return NULL;
                }

                void resetSkin() { _skin.reset(); }

                /**
                 * Set widget skin
                 * @param s shared pointer to the new skin
                 */
                void setSkin(const boost::shared_ptr<skin> &s) { _skin = s; }

                /**
                 * Get pointer to the layout
                 */
                boost::shared_ptr<layout> getLayout() const { return _layout; }
                void resetLayout() { _skin.reset(); }

                /**
                 * Set widget layout
                 * @param lay shared pointer to the new layout
                 */
                void setLayout(const boost::shared_ptr<layout> &lay) { _layout = lay; }

                // TODO ?
                bool internal() const { return _internal; }

                /**
                 * Get map of current widget children
                 */
                const widgetMap &children() const { return _children; }

                /**
                 * Get widget's parent
                 */
                widget *parent() const { return _parent; }

                /**
                 * Get widget name
                 */
                const string &name() const { return _name; }

                /**
                 * \brief Get full name
                 *
                 * Returns name concatented with parent's(if exists) name
                 */
                string fullName() const
                {
                    if (_parent)
                        return _parent->fullName() + "::" + _name;
                    return _name;
                }

                /**
                 * \brief Smart widget pointer
                 *
                 * If widget has parent -> return pointer to the widget
                 * If not -> return pointer to the parent's child
                 */
                widgetPtr smartPtr() const
                {
                    if (!_parent)
                        return widgetPtr();
                    return _parent->childPtr(_name);
                }

                // TODO
                virtual widget *select()
                {
                    return selectParent ? (_parent ? _parent->select() : this) : this;
                }

                /**
                 * Constructor
                 * @param name widget name
                 * @internal TODO
                 */
                widget(const string &name, bool internal = false);

                /**
                 * Destructor
                 */
                virtual ~widget();

                // Mostly for read purposes
                // Write purposes supported, but usage of update() is recommended
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

                // User data
                boost::any misc;

                vec4 clipRect() const;

                /**
                 * Make given widget active
                 * @param c widget to use
                 */
                void bringChildToFront(widget &c);

                /**
                 * \brief Makes structure of parent->child active
                 *
                 * It's recursive function which goes through
                 * all parents and makes their child active
                 */
                void bringToFront()
                {
                    if (_parent)
                    {
                        _parent->bringChildToFront(*this);
                        _parent->bringToFront(); // Recurse for parents
                    }
                }

                // TODO:
                bool contains(const vec2 &p) const;

                bool within(const vec4 &v) const;

                /**
                 * Update widget
                 * @param name widget to update
                 */
                virtual widget &update(const string &name, const boost::any &val = boost::any());

                /**
                 * Get widget attribute
                 * @param name attribute to get
                 */
                virtual boost::any attrib(const string &name) const;

                // Derived position
                vec2 derivedPosition() const;
                virtual vec2 skinOffset(const align_t &a = A_TL) const { return vec2(0, 0); }
                virtual string getSkinType() const { return "widget"; }
                vec2 childrenPosition() const { return derivedPosition() + skinOffset(); }

                /**
                 * Get widget opacity
                 */
                float alpha() const { return _alpha; }

                /**
                 * Get alpha value due to all child widgets
                 */
                float derivedAlpha() const
                {
                    if (_parent)
                        return _parent->derivedAlpha() * _alpha;
                    return _alpha;
                }

                /**
                 * Get visible value due to all child widgets
                 */
                bool derivedVisible() const
                {
                    if (_parent)
                        return _parent->visible && visible;
                    return visible;
                }

//                vec4 childrenRect() const
//                {
//                    vec2 so = skinOffset();
//                    return vec4(position.x + so.x, position.y + so.y, size.x, size.y);
//                }

                vec4 derivedChildrenRect() const
                {
                    vec2 p = childrenPosition();
                    return vec4(p.x, p.y, size.x, size.y);
                }

                // The following rect() functions give the full rects, including the skin

                vec4 rect() const;
                vec4 derivedRect() const;

                void sizeWithSkin(const vec2 &v);

                // Usually for scrolling, etc.
                virtual vec2 childrenOffset() const { return vec2(0, 0); }

                widgetPtr childInFamilyPtr(const string &name);
                virtual widgetPtr childAtPtr(const vec2 &p) const;
                widgetPtr childPtr(const string &name);

                // Shortcuts to avoid pointers (will return dummy if NULL)
                widget &childInFamily(const string &name);
                widget &child(const string &name);

                // Convenience
                widget &operator[](const string &name) { return child(name); }

                //widgetPtr childPtr(const widgetPtr &ptr);
                virtual widget &child(const widgetPtr &ptr);
                widget &child(widget *ptr)
                {
                    assert(ptr);
                    return child(widgetPtr(ptr));
                }

                // Shorthand
                widget &operator()(const string &name, const boost::any &val = boost::any())
                {
                    return update(name, val);
                }

                // Shorthand
                // Sssumes &ref is a new'd address (should be)
                widget &operator()(widget &ref0) { child(&ref0); return *this; }
                widget &operator()(widget &ref0, widget &ref1) { child(&ref0); child(&ref1); return *this; }
                widget &operator()(widget &ref0, widget &ref1, widget &ref2)
                { child(&ref0); child(&ref1); child(&ref2); return *this; }
                widget &operator()(widget &ref0, widget &ref1, widget &ref2, widget &ref3)
                { child(&ref0); child(&ref1); child(&ref2); child(&ref3); return *this; }
                widget &operator()(widget &ref0, widget &ref1, widget &ref2, widget &ref3, widget &ref4)
                { child(&ref0); child(&ref1); child(&ref2); child(&ref3); child(&ref4); return *this; }
                widget &operator()(widget &ref0, widget &ref1, widget &ref2, widget &ref3, widget &ref4, widget &ref5)
                { child(&ref0); child(&ref1); child(&ref2); child(&ref3); child(&ref4); child(&ref5); return *this; }

                /**
                 * Remove child from parent
                 * @param name child name to remove
                 */
                void removeChild(const string &name);
                void clearChildren(bool clearInternal = false);

                /**
                 * Set event callback function
                 * @param id callback's id
                 * @param func function to apply for callback
                 */
                widget &callback(const string &id, const callbackFunc &func)
                {
                    _callbacks[id] = func;
                    return *this;
                }

                /**
                 * Remove callback
                 * @param id callback's id to remove
                 */
                void removeCallback(const string &id)
                {
                    callbackMap::iterator i = _callbacks.find(id);
                    if (i != _callbacks.end())
                        _callbacks.erase(i);
                }

                /**
                 * Remove all callbacks
                 */
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

        /**
         * Just a dummy widget :)
         */
        class dummyWidget: public widget
        {
            protected:
            public:

                dummyWidget() {}
                ~dummyWidget() {}
        };

        /**
         * Returns height used for the grid in total
         */
        void resizeToFitChildren(widget &w, const vec2 &maxSize = vec2(1024, 1024));

//        template<typename A, typename B>
//        boost::shared_ptr<A> widgetCast(const boost::shared_ptr<B> &p)
//        {
//            return boost::static_pointer_cast<A>(p);
//        }

    }
}

#endif // WIDGET_HPP_INCLUDED
