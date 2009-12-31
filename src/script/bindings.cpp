 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "bindings.hpp"
#include "script.hpp"
#include <chaiscript/chaiscript.hpp>

//#include "game/entity.hpp"
#include <OgreColourValue.h>

#include "data_dir.hpp"
#include "log.hpp"
#include "types.hpp"
#include "action_binder.hpp"
#include "gui/widget.hpp"
#include "game/entity.hpp"
#include "game/level.hpp"

//! from luabind/examples/any_converter.cpp
//namespace
//{
//    template<class T>
//    struct convertAny
//    {
//        static void go(lua_State *L, const boost::any &a)
//        {
//            luabind::detail::convert_to_lua(L, *boost::any_cast<T>(&a));
//        }
//    };
//
//    std::map<const std::type_info*, void(*)(lua_State*, const boost::any&)> anyConverters;
//
//    template<class T>
//    void addAnyConverter()
//    {
//        anyConverters[&typeid(T)] = convertAny<T>::go;
//    }
//
//    void addAnyConverters()
//    {
////        static bool done = false;
////        if (done) return;
////        else done = true;
//
//        using namespace engine;
//        addAnyConverter<int>();
//        addAnyConverter<float>();
//        addAnyConverter<string>();
//        addAnyConverter<bool>();
//        addAnyConverter<vec2>();
//        addAnyConverter<vec4>();
//        addAnyConverter<colour>();
//        addAnyConverter<const char*>();
//    }
//}
//
//namespace luabind
//{
//	namespace converters
//	{
//		yes_t is_user_defined(by_value<boost::any>);
//		yes_t is_user_defined(by_const_reference<boost::any>);
//
//		void convert_cpp_to_lua(lua_State *L, const boost::any &a)
//		{
//			typedef void (*conv_t)(lua_State *L, const boost::any&);
//			conv_t conv = anyConverters[&a.type()];
//			conv(L, a);
//		}
//	}
//}

namespace engine
{

    namespace script
    {

//        namespace
//        {
//            // from luabind docs
//            int add_file_and_line(lua_State* L)
//            {
//                lua_Debug d;
//                lua_getstack(L, 1, &d);
//                lua_getinfo(L, "Sln", &d);
//                std::string err = lua_tostring(L, -1);
//                lua_pop(L, 1);
//                std::stringstream msg;
//                msg << d.short_src << ":" << d.currentline;
//
//                if (d.name != 0)
//                {
//                    msg << "(" << d.namewhat << " " << d.name << ")";
//                }
//                msg << " " << err;
//                lua_pushstring(L, msg.str().c_str());
//                return 1;
//            }
//        }

//        boost::any convert(const luabind::object &o)
//        {
//            switch (luabind::type(o))
//            {
//                case LUA_TNIL:
//                case LUA_TNONE:
//                    return boost::any();
//                break;
//
//                case LUA_TNUMBER:
//                    return luabind::object_cast<float>(o);
//                break;
//
//                case LUA_TSTRING:
//                    return luabind::object_cast<string>(o);
//                break;
//
//                case LUA_TBOOLEAN:
//                    return luabind::object_cast<bool>(o);
//                break;
//
//                case LUA_TUSERDATA:
//                    // custom class
//                    #define TRY_TYPE(T)\
//                    do { if (boost::optional<T> t = luabind::object_cast_nothrow<T>(o))\
//                    {\
//                        return t.get();\
//                    } } while (false)
//                    TRY_TYPE(vec2);
//                    TRY_TYPE(vec3);
//                    TRY_TYPE(vec4);
//                    TRY_TYPE(quat);
//                    TRY_TYPE(colour);
//                    //TRY_TYPE(gui::widget::arrangeInfo);
//                    TRY_TYPE(game::entityPtr);
//                    #undef TRY_TYPE
//                break;
//
//                default:
//                    return boost::any();
//                break;
//            }
//            return boost::any();
//        }
//
//        luabind::object convert(lua_State *L, const boost::any &o)
//        {
//            #define TRY_TYPE(T)\
//            do { if (const T *t = boost::any_cast<T>(&o))\
//            {\
//                return luabind::object(L, *t);\
//            } } while (false)
//            TRY_TYPE(float);
//            TRY_TYPE(double);
//            TRY_TYPE(int);
//            TRY_TYPE(string);
//            TRY_TYPE(bool);
//            TRY_TYPE(vec2);
//            TRY_TYPE(vec3);
//            TRY_TYPE(vec4);
//            TRY_TYPE(quat);
//            TRY_TYPE(colour);
//            TRY_TYPE(game::entityPtr);
//            //TRY_TYPE(gui::widget::arrangeInfo);
//            #undef TRY_TYPE
//
//            return luabind::object();
//        }

//        void prepare(lua_State *L)
//        {
////            static bool alreadySetPFunc = false;
////            if (!alreadySetPFunc)
////            {
////                luabind::set_pcall_callback(&add_file_and_line);
////                alreadySetPFunc = true;
////            }
//
//            //addAnyConverters();
//            luaL_openlibs(L);
//
//            luabind::open(L);
//        }

        void bindGfx(script &src)
        {
            using namespace chaiscript;
        }

        void bindCommon(script &src)
        {
            using namespace chaiscript;

//            module(L)
//            [
//                def("log", &log),
//
//                class_<colour>("colour")
//                    .def(constructor<float, float, float>())
//                    .def(constructor<float, float, float, float>())
//                    .def_readwrite("r", &colour::r)
//                    .def_readwrite("g", &colour::g)
//                    .def_readwrite("b", &colour::b)
//                    .def_readwrite("a", &colour::a)
//            ];
//
//            chaiscript::globals(L)["DATA_DIR"] = DATA_DIR;
        }

//        void _goNoMisc(actionBinder *_this, const string &str)
//        {
//            _this->go(str, boost::any());
//        }
//
//        void _goMisc(actionBinder *_this, const string &str, const chaiscript::object &obj)
//        {
//            _this->go(str, convert(obj));
//        }

//        void _callBindedLuaCallback(const boost::any &misc, chaiscript::object func)
//        {
//            assert(chaiscript::type(func) == LUA_TFUNCTION);
//            func();
//        }
//
//        void _bindLuaCallback(actionBinder *_this, const string &k, const chaiscript::object &func)
//        {
//            if (chaiscript::type(func) != LUA_TFUNCTION)
//            {
//                log("tried to bind \"" + k + "\" to non-function lua object");
//                return;
//            }
//            _this->bind(k, boost::bind(&_callBindedLuaCallback, boost::any(), func));
//        }

        void bindActionBinder(script &src)
        {
            using namespace chaiscript;

//            module(L)
//            [
//                class_<actionBinder>("actionBinder")
//                    //.def(constructor<>())
//                    //.def("bind", (void(actionBinder::*)(const string&, const string&))&actionBinder::bind)
//                    .def("bind", &_bindLuaCallback)
//                    .def("unbindAll", &actionBinder::unbindAll)
//                    .def("go", &_goNoMisc)
//                    .def("go", &_goMisc)
//            ];
        }

    }

}
