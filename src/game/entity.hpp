 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef ENTITY_HPP_INCLUDED
#define ENTITY_HPP_INCLUDED

#include <map>
#include <vector>
#include <algorithm>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "id.hpp"
#include "types.hpp"

#include "log.hpp"

namespace levelFormat
{
    class entity;
}

/**
 * @namespace game
 * @brief Consist of tools for building game world
 */
namespace game
{
	/// Shared pointer to the entity
    typedef boost::shared_ptr<class entity> entityPtr;

    class world;

    using engine::interfaceType;
    using engine::interfaceList;

    /**
     * @class entity
     * @brief Represents some object-entity in game world
     */
    class entity
    {
        public:
			/// Entity type
            static const interfaceType TYPE;

        protected:
			/// Name of the entity
            engine::string _name;

            /// List of interfaces
            interfaceList _interfaces;

            friend class world;
            /// Pointer to the class we will use entity in
            world *_parent;

            // WTF?
            boost::shared_ptr<levelFormat::entity> _initialState;

            /// List of entities
            static engine::idGen _idGen;

        public:
			/**
			 * @brief Get entity parent
			 * @return world object this entity refer for
			 */
            world *parent() const { return _parent; }

            /**
             * @brief Change entity parent world
             * @param w new world
             */
            virtual void willChangeTo(world *w) {}

            /**
             * Constructor
             */
            entity(const engine::string &n);

            /**
             * Destructor
             */
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

            // TODO: comment functions
            boost::shared_ptr<levelFormat::entity> copyOfInitialState() const;

            boost::shared_ptr<levelFormat::entity> currentState();

            /**
             * @brief Get entity's name
             */
            const engine::string &name() const { return _name; }

            /**
             * @brief Set new name for the current entity
             * @param str new name
             */
            void name(const engine::string &str);

            /**
             * @brief Get list of interfaces
             */
            const interfaceList &interfaces() const { return _interfaces; }

            /**
             * @brief Check availability of some interface
             * @param t interface type to check
             */
            bool hasInterface(const interfaceType &t) const
            {
                return std::find(_interfaces.begin(), _interfaces.end(), t) != _interfaces.end();
            }

            /**
             * @brief Get interface type
             */
            const interfaceType &type() const
            {
                assert(!_interfaces.empty());
                return _interfaces.back();
            }

            /**
             * @brief Get pointer for the entity with given name
             * @param name need entity name
             * @return pointer for the entity
             */
            virtual entityPtr clone(const engine::string &name)
            {
                return entityPtr(new entity(name));
            }

            /**
             * @brief Smart pointer for the current entity
             */
            entityPtr smartPtr() const;

            /**
             * @brief Reset original state
             */
            virtual void reset();

            /**
             * @brief Used with global application cycle
             */
            virtual void tick(engine::real dt);

            /**
             * TOOD: maybe loading entity using format from levelFormat class
             */
            // TODO All this stuff
            virtual void load(const levelFormat::entity &desc);
            void loadAsInitialState(const levelFormat::entity &desc);
            virtual void save(levelFormat::entity &desc);

    };

    class entityFactory;
    /// Shared pointer for the entity factory
    typedef boost::shared_ptr<entityFactory> entityFactoryPtr;

    /**
     * @brief Cast entity to given type
     * @param ent pointer for the entity to cast
     * @param A new entity type
     */
    template<typename A>
    boost::shared_ptr<A> entityCast(const entityPtr &ent)
    {
        if (!ent)
            return boost::shared_ptr<A>();
        return boost::static_pointer_cast<A>(ent);
    }

    /**
     * @brief Cast entity to given type due to given interface
     * @param ent pointer for the entity to cast
     * @param type interface type
     * @param A new entity type
     */
    template<typename A>
    boost::shared_ptr<A> entityCast(const entityPtr &ent, const interfaceType &type)
    {
        if (!ent)
            return boost::shared_ptr<A>();
        if (!ent->hasInterface(type))
        {
            engine::log("Cannot cast entity " + ent->name() + " of type " +
                boost::lexical_cast<engine::string>(ent->type()) + " to type " +
                boost::lexical_cast<engine::string>(type));
            return boost::shared_ptr<A>();
        }
        return entityCast<A>(ent);
    }

    /// For fast entity casting
    #define ENTITY_CAST(T, ent) entityCast<T>((ent), T::TYPE)

    /**
     * @class entityFactory
     * @brief Entity factory
     * @see class entity
     * @remarks for more information
     * visit: http://en.wikipedia.org/wiki/Factory_method_pattern
     */
    class entityFactory
    {
        protected:
            typedef std::map<interfaceType, entityFactoryPtr> factoryList;

            /// List of factories
            factoryList factories;

            /// Interface type
            interfaceType _type;

        public:
			/**
			 * @brief Get interface type
			 */
            const interfaceType &type() const { return _type; }

            /**
             * Constructor
             */
            entityFactory(const interfaceType &t = entity::TYPE):
                _type(t) {}

            /**
             * Destructor
             */
            virtual ~entityFactory() {}

            /**
             * @brief Register custom factory
             */
            void add(const entityFactoryPtr &f);

            // retrieve custom factory
//            entityFactoryPtr operator[](const engine::string &t) const;

            /**
             * Spawn an entity
             */
            entityPtr create(const interfaceType &type, const engine::string &name) const;

            /**
             * @brief Create new entity for the factory
             * @param name new entity name
             * @return pointer to the created entity
             */
            virtual entityPtr create(const engine::string &name) const;
    };

//    template<typename A>
//    boost::shared_ptr<A> factoryCast(const entityFactoryPtr &p)
//    {
//        return boost::static_pointer_cast<A>(p);
//    }
}

#endif // ENTITY_HPP_INCLUDED
