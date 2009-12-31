 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef ENTITY_HPP_INCLUDED
#define ENTITY_HPP_INCLUDED

#include <map>
#include <vector>
#include <algorithm>

#include <boost/shared_ptr.hpp>

#include "id.hpp"
#include "types.hpp"

#include "log.hpp"

namespace levelFormat
{
    class entity;
}

namespace game
{

    typedef boost::shared_ptr<class entity> entityPtr;

    class world;

    using engine::interfaceType;
    using engine::interfaceList;

    class entity
    {
        public:

            static const interfaceType TYPE;

        protected:

            engine::string _name;

            interfaceList _interfaces;

            friend class world;
            world *_parent;

            boost::shared_ptr<levelFormat::entity> _initialState;

            static engine::idGen _idGen;

        public:

            world *parent() const { return _parent; }
            virtual void willChangeTo(world *w) {}

            entity(const engine::string &n);
            virtual ~entity();

            // little utility
            void setCurrentStateAsInitial()
            {
                assert(_initialState);
                save(*_initialState);
            }

            boost::shared_ptr<levelFormat::entity> initialState() const
            {
                return _initialState;
            }

            boost::shared_ptr<levelFormat::entity> copyOfInitialState() const;

            boost::shared_ptr<levelFormat::entity> currentState();

            const engine::string &name() const { return _name; }
            void name(const engine::string &str);
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

            virtual entityPtr clone(const engine::string &name)
            {
                return entityPtr(new entity(name));
            }

            entityPtr smartPtr() const;

            virtual void reset(); // reset original state

            virtual void tick(engine::real dt);

            virtual void load(const levelFormat::entity &desc);
            void loadAsInitialState(const levelFormat::entity &desc);
            virtual void save(levelFormat::entity &desc);

    };

    class entityFactory;
    typedef boost::shared_ptr<entityFactory> entityFactoryPtr;

    template<typename A>
    boost::shared_ptr<A> entityCast(const entityPtr &ent)
    {
        if (!ent)
            return boost::shared_ptr<A>();
        return boost::static_pointer_cast<A>(ent);
    }

    template<typename A>
    boost::shared_ptr<A> entityCast(const entityPtr &ent, const interfaceType &type)
    {
        if (!ent)
            return boost::shared_ptr<A>();
        if (!ent->hasInterface(type))
        {
            engine::log("cannot cast entity " + ent->name() + " of type " +
                boost::lexical_cast<engine::string>(ent->type()) + " to type " +
                boost::lexical_cast<engine::string>(type));
            return boost::shared_ptr<A>();
        }
        return entityCast<A>(ent);
    }

    // for conveniency
    // assumes <type>Ptr naming
    #define ENTITY_CAST(T, ent) entityCast<T>((ent), T::TYPE)

    class entityFactory
    {
        protected:

            typedef std::map<interfaceType, entityFactoryPtr> factoryList;
            factoryList factories;

            interfaceType _type;

        public:

            const interfaceType &type() const { return _type; }

            entityFactory(const interfaceType &t = entity::TYPE):
                _type(t) {}

            // virtual destructor
            virtual ~entityFactory() {}

            // register custom factory
            void add(const entityFactoryPtr &f);
            // retrieve custom factory
//            entityFactoryPtr operator[](const engine::string &t) const;
            // spawn an entity
            entityPtr create(const interfaceType &type, const engine::string &name) const;
            virtual entityPtr create(const engine::string &name) const;
    };

//    template<typename A>
//    boost::shared_ptr<A> factoryCast(const entityFactoryPtr &p)
//    {
//        return boost::static_pointer_cast<A>(p);
//    }

}

#endif // ENTITY_HPP_INCLUDED
