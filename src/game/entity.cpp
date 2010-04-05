 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "entity.hpp"
#include "log.hpp"

#include "world.hpp"
#include "safe_any.hpp"
#include "entity_types.hpp"

#include "level_format.pb.h"

namespace game
{
    engine::idGen entity::_idGen;

    // Default entity type
    const interfaceType entity::TYPE = ENTITY_ENTITY;

    entity::entity(const engine::string &n):
        _name(n),
        _parent(NULL),
        _initialState(new levelFormat::entity())
    {
        // Ensure genuine name
        _name = _idGen(_name);

        if (_name != n)
            engine::log("Entity with name " + n + " already exists, renaming to " + _name);

        _interfaces.push_back(entity::TYPE);
    }

    entity::~entity()
    {
        // Allow same name to be used
        _idGen -= _name;
    }

    void entity::name(const engine::string &str)
    {
        if (_name != str)
        {
            engine::string before = _name;

            _idGen -= _name;
            _name = _idGen(str);

            if (_parent)
                _parent->rename(before, _name);
        }
    }

    void entity::tick(engine::real dt)
    {
    }

    void entity::reset()
    {
        assert(_initialState);
        load(*_initialState);
    }

    void entity::load(const levelFormat::entity &desc)
    {
    }

    void entity::loadAsInitialState(const levelFormat::entity &desc)
    {
        load(desc);
        assert(_initialState);
        _initialState->CopyFrom(desc);
    }

    void entity::save(levelFormat::entity &desc)
    {
        desc.set_type(type());
        desc.set_name(name());
    }

    boost::shared_ptr<levelFormat::entity> entity::copyOfInitialState() const
    {
        assert(_initialState);
        boost::shared_ptr<levelFormat::entity> ret(new levelFormat::entity());
        ret->CopyFrom(*_initialState);
        return ret;
    }

    boost::shared_ptr<levelFormat::entity> entity::currentState()
    {
        boost::shared_ptr<levelFormat::entity> ret(new levelFormat::entity());
        save(*ret);
        return ret;
    }

    entityPtr entity::smartPtr() const
    {
        if (!_parent)
        {
            engine::log("Cannot get smartPtr() to entity \"" + _name + "\" in NULL world");
            return entityPtr();
        }
        return _parent->get(_name);
    }

    void entityFactory::add(const entityFactoryPtr &f)
    {
        factories[f->type()] = f;
    }

    entityPtr entityFactory::create(const interfaceType &type, const engine::string &name) const
    {
        using namespace engine;

        factoryList::const_iterator i = factories.find(type);
        if (i == factories.end())
        {
            engine::log("Entity factory for type \"" +
                boost::lexical_cast<engine::string>(type)+ "\" not registered");
            return entityPtr();
        }

        return i->second->create(name);
    }

    entityPtr entityFactory::create(const engine::string &name) const
    {
        engine::log("Cannot create entity \"" + name + "\" because this factory is generic");
        return entityPtr();
    }
}
