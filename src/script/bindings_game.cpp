 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <chaiscript/chaiscript.hpp>

#include "bindings.hpp"

//#include "game/entity.hpp"
#include <OgreColourValue.h>

#include "data_dir.hpp"
#include "log.hpp"

#include "types.hpp"

#include "gui/sheet.hpp"

#include "snd/system.hpp"

#include "game/entity.hpp"
#include "game/generic_prop.hpp"
#include "game/level.hpp"
#include "game/portal.hpp"
#include "game/world.hpp"
#include "game/trigger.hpp"
#include "game/marker.hpp"

namespace engine
{

    namespace script
    {

        game::entityPtr _factoryCreate(const game::entityFactory &_this,
            const game::interfaceType &type, const engine::string &name)
        //, const luabind::object &table)
        {
            return _this.create(type, name);
        }
//        {
//            using namespace luabind;
//
//            if (type(table) != LUA_TTABLE)
//            {
//                log("try to factory:create with non-table");
//                return game::entityPtr();
//            }
//
//            engine::parameters params;
//            for (iterator i(table), end; i != end; ++i)
//            {
//                const object &keyObj = i.key();
//                if (type(keyObj) != LUA_TSTRING)
//                {
//                    log("key invalid (factory create of type \"" + _this.type() + "\")");
//                    continue;
//                }
//                string key = object_cast<string>(keyObj);
//                const object &obj = *i;
//                params.set(key, convert(obj));
//            }
//
//            return _this(params);
//        }

//        void _entityUpdate(game::entity &_this, const luabind::object &table)
//        {
//            using namespace luabind;
//
//            if (type(table) != LUA_TTABLE)
//            {
//                log("tried to entity:update() with non-table type");
//                return;
//            }
//
//            engine::parameters params;
//            for (iterator i(table), end; i != end; ++i)
//            {
//                const object &keyObj = i.key();
//                if (type(keyObj) != LUA_TSTRING)
//                {
//                    log("key invalid (entity update of \"" + _this.name() + "\")");
//                    continue;
//                }
//                string key = object_cast<string>(keyObj);
//                const object &obj = *i;
//                params.set(key, convert(obj));
//            }
//
//            _this.update(params);
//        }

//        luabind::object _entityAttrib(lua_State *L, game::entity &_this, const engine::string &name)
//        {
//            boost::any ret = _this.attrib(name);
//            if (ret.empty())
//                return luabind::object();
//            return convert(L, ret);
//        }

        game::genericPropPtr _toGenericProp(const game::entityPtr &ent)
        {
            using namespace game;
            return ENTITY_CAST(genericProp, ent);
        }

        game::portalPtr _toPortal(const game::entityPtr &ent)
        {
            using namespace game;
            return ENTITY_CAST(portal, ent);
        }

        game::triggerPtr _toTrigger(const game::entityPtr &ent)
        {
            using namespace game;
            return ENTITY_CAST(trigger, ent);
        }

        game::markerPtr _toMarker(const game::entityPtr &ent)
        {
            using namespace game;
            return ENTITY_CAST(marker, ent);
        }

        void bindGame(script &scr)
        {
            using namespace chaiscript;
            using namespace game;

//            module(L, "game")
//            [
//                def("toGenericProp", &_toGenericProp)
//                ,def("toPortal", &_toPortal)
//                ,def("toTrigger", &_toTrigger)
//                ,def("toMarker", &_toMarker)
//
//                ,class_<world>("world")
//                    .def("add", &world::add)
//                    .def("remove", &world::remove)
//                    .def("get", &world::get)
//                    .def("reset", &world::reset)
//                    .def("clear", &world::clear)
//
//                ,class_<level>("level")
//                    .def("reset", &level::reset)
//                    .def_readwrite("gameWorld", &level::gameWorld)
//                    .def_readwrite("physWorld", &level::physWorld)
//                    .def_readwrite("entFactory", &level::entFactory)
//                    .def_readwrite("script", &level::script)
//                    .def_readwrite("guiSheet", &level::guiSheet)
//                    .def("soundSys", &level::soundSys)
//
//                ,class_<entity, entityPtr>("entity")
//                    .def("name", (const engine::string &(entity::*)() const)&entity::name)
//                    .def("name", (void (entity::*)(const engine::string&))&entity::name)
//                    .def("type", &entity::type)
//                    .def("setCurrentStateAsInitial", &entity::setCurrentStateAsInitial)
//                    .def("reset", &entity::reset)
//                    .def("hasInterface", &entity::hasInterface)
//                    .enum_("interfaceType")
//                    [
//                        value("GENERIC_PROP", (unsigned)genericProp::TYPE)
//                        ,value("PORTAL", (unsigned)portal::TYPE)
//                        ,value("TRIGGER", (unsigned)trigger::TYPE)
//                        ,value("MARKER", (unsigned)marker::TYPE)
//                    ]
//
//
//                ,class_<genericProp, entity, genericPropPtr>("genericProp")
////                    .enum_("interfaceType")
////                    [
////                        value("TYPE", (unsigned)ENTITY_GENERIC_PROP)
////                    ]
//                    // set
//                    .def("mass", (void (genericProp::*)(float))&genericProp::mass)
//                    // get
//                    .def("mass", (float (genericProp::*)() const)&genericProp::mass)
//                    // set
//                    .def("vel", (void (genericProp::*)(const vec3&))&genericProp::vel)
//                    // get
//                    .def("vel", (vec3 (genericProp::*)() const)&genericProp::vel)
//                    // set
//                    .def("pos", (void (genericProp::*)(const vec3&))&genericProp::pos)
//                    // get
//                    .def("pos", (vec3 (genericProp::*)() const)&genericProp::pos)
//                    // set
//                    .def("scale", (void (genericProp::*)(const vec3&))&genericProp::scale)
//                    // get
//                    .def("scale", (vec3 (genericProp::*)() const)&genericProp::scale)
//                    // set
//                    .def("omega", (void (genericProp::*)(const vec3&))&genericProp::omega)
//                    // get
//                    .def("omega", (vec3 (genericProp::*)() const)&genericProp::omega)
//                    // set
//                    .def("rot", (void (genericProp::*)(const quat&))&genericProp::rot)
//                    // get
//                    .def("rot", (quat (genericProp::*)() const)&genericProp::rot)
//                    // get
//                    .def("collidable", (bool (genericProp::*)() const)&genericProp::collidable)
//                    // set
//                    .def("collidable", (void (genericProp::*)(bool))&genericProp::collidable)
//                    // get
//                    .def("active", (bool (genericProp::*)() const)&genericProp::active)
//                    // set
//                    .def("active", (void (genericProp::*)(bool))&genericProp::active)
//                    .def("loadScript", &genericProp::loadScript)
//                    .def("getScript", &genericProp::getScript)
//                    .def("origMaterial", &genericProp::origMaterial)
//                    .def("setMaterial", &genericProp::setMaterial)
//                    .def_readwrite("gravity", &genericProp::gravity)
//
//                ,class_<portal, genericProp, portalPtr>("portal")
//                    .def("connection", (void (portal::*)(const portalPtr&))&portal::connection)
//                    .def("connectionName", &portal::connectionName)
//
//                ,class_<trigger, genericProp, triggerPtr>("trigger")
//                    .def_readwrite("onEnter", &trigger::onEnter)
//                    .def_readwrite("onExit", &trigger::onExit)
//
//                ,class_<marker, genericProp, markerPtr>("marker")
//
//                ,class_<entityFactory>("entityFactory")
//                    .def("create", &_factoryCreate)
//            ];
        }


    }

}
