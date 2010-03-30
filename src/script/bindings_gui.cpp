 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <chaiscript/chaiscript.hpp>

#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>

#include <OgreColourValue.h>

#include "bindings.hpp"

#include "gui/sheet.hpp"
#include "gui/widget.hpp"
//#include "gui/window.hpp"
#include "gui/panel.hpp"
/*#include "gui/scrollbar.hpp"
#include "gui/text_box.hpp"
#include "gui/scrollable_panel.hpp"
#include "gui/console.hpp"*/
#include "gui/button.hpp"
/*#include "gui/file_dialog.hpp"
#include "gui/drop_down_menu.hpp"*/
#include "gui/skin.hpp"
//#include "gui/check_box.hpp"
#include "gui/layout.hpp"

#include "log.hpp"
#include "types.hpp"

namespace engine
{

    namespace script
    {

//        boost::reference_wrapper<engine::gui::widget> _child(
//            engine::gui::widget &_this, const engine::string &name)
//        {
//            return boost::ref(_this.child(name));
//        }
//
//        boost::reference_wrapper<engine::gui::widget> _childInFamily(
//            engine::gui::widget &_this, const engine::string &name)
//        {
//            return boost::ref(_this.childInFamily(name));
//        }

//        void _widgetLuaCallback(const gui::event &e, luabind::object func)
//        {
//            assert(luabind::type(func) == LUA_TFUNCTION);
//            try
//            {
//                func(e.caller.smartPtr(), convert(func.interpreter(), e.misc));
//            }
//            catch(const luabind::error &e)
//            {
//                log(string(e.what()) + " :: " + lua(func.interpreter()).error());
//            }
//        }
//
//        gui::widgetPtr _makeWidget(const string &type, const string &name)
//        {
//            using namespace gui;
//
//            if (type == "panel") return widgetPtr(new panel(name));
//            else if (type == "window") return widgetPtr(new window(name));
//            else if (type == "text") return widgetPtr(new text(name));
//            else if (type == "scrollbar") return widgetPtr(new scrollbar(name));
//            else if (type == "textBox") return widgetPtr(new textBox(name));
//            else if (type == "console") return widgetPtr(new console(name));
//            else if (type == "scrollablePanel") return widgetPtr(new scrollablePanel(name));
//            else if (type == "fileDialog") return widgetPtr(new fileDialog(name));
//            else if (type == "button") return widgetPtr(new button(name));
//            else if (type == "dropDownMenu") return widgetPtr(new dropDownMenu(name));
//            else if (type == "checkBox") return widgetPtr(new checkBox(name));
//
//            log("could not _makeWidget() \"" + name + "\" of type \"" + type + "\"");
//
//            return widgetPtr();
//        }
//
//        void _updateWidget(gui::widget &_this, const luabind::object &table)
//        {
//            using namespace luabind;
//
//            if (type(table) != LUA_TTABLE)
//            {
//                log("tried to widget:update() with non-table type");
//                return;
//            }
//
//            for (iterator i(table), end; i != end; ++i)
//            {
//                const object &keyObj = i.key();
//                if (type(keyObj) != LUA_TSTRING)
//                {
//                    if (type(*i) == LUA_TTABLE)
//                    {
//                        // nested table, probably to avoid ordering issues in
//                        // the lua tables... just iterate through it
//                        _updateWidget(_this, *i);
//                    }
//                    else
//                        log("key invalid (widget update of \"" + _this.name() + "\")");
//
//                    continue;
//                }
//
//                string key = object_cast<string>(keyObj);
//
//                if (key == "type")
//                {
//                    // we're recursing, and we encountered our type (used for
//                    // an upper widget that adds us as a child)
//                    // just ignore this so we don't complain about "type" not found
//                    // in update())
//                    continue;
//                }
//
//                switch (type(*i))
//                {
//                    case LUA_TTABLE:
//                    {
//                        // another table, must be a child
//                        string childName = key;
//                        // if we already have a child with this name, just update
//                        // its properties
//                        if (gui::widgetPtr child = _this.childPtr(childName))
//                        {
//                            _updateWidget(*child, *i);
//                        }
//                        else // otherwise, create the child as such
//                        {
//                            const object &typeObj = (*i)["type"];
//                            if (type(typeObj) != LUA_TSTRING)
//                            {
//                                log("tried to make child \"" + childName + "\" of \"" + _this.name() + "\" with invalid type");
//                                continue;
//                            }
//                            string childType = object_cast<string>(typeObj);
//
//                            // recurse and set table of this child widget
//                            gui::widgetPtr c = _makeWidget(childType, childName);
//                            if (c)
//                                _updateWidget(_this.child(c), *i);
//                        }
//                    }
//                    break;
//
//                    // callbacks can be bound as lua functions
//                    case LUA_TFUNCTION:
//                    {
//                        const object &obj = *i;
//                        _this.callback(key, boost::bind(&_widgetLuaCallback, _1, obj));
//                    }
//                    break;
//
//                    default:
//                    {
//                        const object &obj = *i;
//                        _this.update(key, convert(obj));
//                    }
//                    break;
//                }
//            }
//        }

        void bindGUI(script &scr)
        {
            using namespace chaiscript;

            using namespace gui;

//            module(L, "gui")
//            [
//                class_<skin, skinPtr>("skin")
//                    .def(constructor<const string&>())
//                    .def("getTexture", &skin::getTexture)
//
//                ,class_<layout, layoutPtr>("layout")
//                    .def("addWidget", &layout::addWidget)
//                    .def("addLayout", &layout::addLayout)
//
//                ,class_<verticalLayout, layout, layoutPtr>("verticalLayout")
//                    .def(constructor<>())
//
//                ,class_<horizontalLayout, layout, layoutPtr>("horizontalLayout")
//                    .def(constructor<>())
//
//                ,class_<widget, widgetPtr>("widget")
//                    .def("child", &widget::childPtr)
//                    .def("childInFamily", &widget::childInFamilyPtr)
//                    .def("update", &_updateWidget)
//                    .def("name", &widget::name)
//                    .def("setSkin", &widget::setSkin)
//                    .def("resetSkin", &widget::resetSkin)
//                    .def("setLayout", &widget::setLayout)
//                    .def("resetLayout", &widget::resetLayout)
//                    .def("getLayout", &widget::getLayout)
//                    .def_readwrite("visible", &widget::visible)
//                    .def_readwrite("position", &widget::position)
//                    .def_readwrite("size", &widget::size)
//                    .def_readwrite("size", &widget::size)
//                    .def_readwrite("tip", &widget::tip)
//                    .def("rect", &widget::rect)
//                    .def("sizeWithSkin", &widget::sizeWithSkin)
//                    .def("contains", &widget::contains)
//                    .def("clearCallbacks", &widget::clearCallbacks)
//                    .def("removeCallback", &widget::removeCallback)
//                    .def("clearChildren", &widget::clearChildren)
//                    .def("removeChild", &widget::removeChild)
////                    .def("resizeToFitChildren", &resizeToFitChildren)
//                    //.def("arrange", &widget::arrange)
//                    .enum_("specialKey")
//                    [
//                        value("SK_UP", (unsigned)SK_UP),
//                        value("SK_DOWN", (unsigned)SK_DOWN),
//                        value("SK_LEFT", (unsigned)SK_LEFT),
//                        value("SK_RIGHT", (unsigned)SK_RIGHT),
//                        value("SK_BACKSPACE", (unsigned)SK_BACKSPACE),
//                        value("SK_ENTER", (unsigned)SK_ENTER),
//                        value("SK_ESCAPE", (unsigned)SK_ESCAPE)
//                    ]
//
//                ,class_<sheet, widget, sheetPtr>("sheet")
//
////                ,class_<sheet, widget>("sheet")
////                ,class_<panel, widget>("panel")
////                ,class_<window, widget>("window")
////                ,class_<text, widget>("text")
////                ,class_<scrollbar, widget>("scrollbar")
////                ,class_<titlebar, widget>("titlebar")
//            ];
        }

    }

}
