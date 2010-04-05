 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "phys/body.hpp"
#include "phys/world.hpp"

#include "entity.hpp"
#include "trigger.hpp"
#include "data_dir.hpp"
#include "safe_any.hpp"

#include "entity_types.hpp"
#include "level_format.pb.h"

namespace game
{
    const interfaceType trigger::TYPE = ENTITY_TRIGGER;

    trigger::trigger(
        const engine::string &name,
        const genericProp::constructParams &params,
        const engine::script::script &script):
        genericProp(name, params),
        _script(script)
    {
        _interfaces.push_back(trigger::TYPE);

        visible(!params.inGame);
        collidable(false);
    }

    trigger::~trigger()
    {
    }

    void trigger::processContacts(engine::phys::body &o,
        const engine::phys::contactList &contacts, engine::phys::contactList &rejections, float dt)
    {
        using namespace engine;
        using namespace engine::phys;

        genericProp::processContacts(o, contacts, rejections, dt);

        // No entity associated with body
        if (o.misc.empty())
            return;

        // Not an entity?
        entity *ent = SAFE_ANY_CAST_T(entity*, o.misc, NULL);
        if (!ent)
            return;

        entityPtr entPtr = ent->smartPtr();
        if (!entPtr)
            return;

        touchInfoList::iterator i = _touches.find(&o);
        if (i == _touches.end())
        {
            touchInfo info = { entPtr, o.autoSleep(), 0 };
            _touches[&o] = info;
            // Don't let the contactee fall asleep or we might get
            // a false exit
            o.autoSleep(false);

            // Don't call in the collision callback so we don't
            // accidentally crash due to multi-threading
            _entered.push_back(entPtr);
//            _callback(onEnter, entPtr);
        }
        else
        {
            i->second.timeSinceContact -= dt;
        }
    }

    void trigger::_callback(const engine::string &str, const entityPtr &ent)
    {
        if (str.empty() || !_parent)
            return;

        entityPtr ptr = smartPtr();
        assert(ptr);

        triggerPtr thisPtr = ENTITY_CAST(trigger, ptr);
        assert(thisPtr);

        // TODO: uncomment when script will be done
//        engine::lua::callFunc(_lua, str, thisPtr, ent);
//        // do garbage collections since we passed in smart pointers
//        _lua.gc();
    }

    void trigger::tick(engine::real dt)
    {
        genericProp::tick(dt);

        BOOST_FOREACH(entityPtr &i, _entered)
            _callback(onEnter, i);
        _entered.clear();

        std::list<touchInfoList::iterator> toRemove;
        for (touchInfoList::iterator i = _touches.begin(); i != _touches.end(); ++i)
        {
            touchInfo &info = i->second;
            info.timeSinceContact += dt;

            // No contact for more than 2 physics steps
            if (info.timeSinceContact > _world.freq() * 2)
            {
                // Prepare to remove this object
                toRemove.push_back(i);

                assert(info.ent);

                _callback(onExit, info.ent);

                i->first->autoSleep(info.wasAutoSleep);
            }
        }

        BOOST_FOREACH(const touchInfoList::iterator &i, toRemove)
            _touches.erase(i);
    }

    void trigger::load(const levelFormat::entity &desc)
    {
        genericProp::load(desc);

        if (!desc.has_trigger_info())
        {
            engine::log("trigger " + _name + " has no trigger_info");
            return;
        }

        const levelFormat::triggerInfo &info = desc.trigger_info();

        if (info.has_on_enter())
            onEnter = info.on_enter();

        if (info.has_on_exit())
            onExit = info.on_exit();
    }

    void trigger::save(levelFormat::entity &desc)
    {
        genericProp::save(desc);

        levelFormat::triggerInfo *info = desc.mutable_trigger_info();

        info->set_on_enter(onEnter);
        info->set_on_exit(onExit);
    }

    void trigger::reset()
    {
        genericProp::reset();

        _entered.clear();
        _touches.clear();
    }

    entityPtr triggerFactory::create(const engine::string &name) const
    {
        using namespace engine;

        triggerPtr ent(new trigger(name, _params, _script));

        return ent;
    }
}
