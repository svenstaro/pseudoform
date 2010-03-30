 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/foreach.hpp>

#include "world.hpp"
#include "entity.hpp"
#include "types.hpp"
#include "log.hpp"
#include "generic_prop.hpp"
#include "safe_any.hpp"

namespace game
{
    world::world()
    {
    }

    world::~world()
    {
        clear();
    }

    void world::add(const entityPtr &ent)
    {
        if (ent)
        {
            if (ent->_parent)
            {
                // Do not add if it already belongs to a world
                engine::log("entity " + ent->name() + " already belongs to a world");
                return;
            }

            _ents[ent->name()] = ent;

            // Set us as it's world
            ent->willChangeTo(this);
            ent->_parent = this;
        }
        else
        {
            engine::log("Tried to add null entity to world");
        }
    }

    entityPtr world::remove(const engine::string &name)
    {
        entityList::iterator i = _ents.find(name);
        if (i != _ents.end())
        {
            entityPtr ptr = i->second; // Save the entity
            _ents.erase(i); // Remove it

            // reset the world
            ptr->willChangeTo(NULL);
            ptr->_parent = NULL;

            return ptr; // Return it to user
        }
        engine::log("Cannot remove entity\"" + name + "\" from world because it does not exist");
        return entityPtr();
    }

    entityPtr world::get(const engine::string &name) const
    {
        entityList::const_iterator i = _ents.find(name);
        if (i != _ents.end())
            return i->second;
        engine::log("Could not find entity with name \"" + name + "\"");
        return entityPtr();
    }

    void world::tick(engine::real dt) const
    {
        const engine::real MIN_FPS = 10.0;
        if (dt > 1.0 / MIN_FPS)
            dt = 1.0 / MIN_FPS;

        BOOST_FOREACH(const entityList::value_type &i, _ents)
            i.second->tick(dt);
    }

    void world::clear()
    {
        BOOST_FOREACH(entityList::value_type &i, _ents)
        {
            i.second->willChangeTo(NULL);
            i.second->_parent = NULL;
        }
        _ents.clear();
    }

    void world::reset()
    {
        BOOST_FOREACH(const entityList::value_type &i, _ents)
            i.second->reset();
    }

    void world::rename(const engine::string &before, const engine::string &now)
    {
        assert(before != now);
        entityList::iterator i = _ents.find(before);
        if (i != _ents.end())
        {
            _ents[now] = i->second;
            _ents.erase(i);
        }
    }

    engine::aabb_t world::aabb() const
    {
        using namespace engine;

        aabb_t ret(vec3(-1, 1, 1), vec3(1, 1, 1));

        BOOST_FOREACH(const entityList::value_type &i, _ents)
        {
            if (!i.second->hasInterface(genericProp::TYPE))
                continue;

            genericPropPtr gp = ENTITY_CAST(genericProp, i.second);

            assert(gp);

            ret.merge(gp->aabb());
        }

        return ret;
    }
}
