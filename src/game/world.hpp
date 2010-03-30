 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef GAME_WORLD_HPP_INCLUDED
#define GAME_WORLD_HPP_INCLUDED

#include <map>

#include <boost/shared_ptr.hpp>

#include "entity.hpp"
#include "types.hpp"

namespace game
{

    typedef std::map<engine::string, entityPtr> entityList;
//    typedef std::map<engine::string, entityList> entityListByType;

    /**
     * @class world
     * @brief Represents game world
     */
    class world
    {
        private:
			/// List of entities in world
            entityList _ents;
//            entityListByType _entsByType;

        public:
            /**
             * Constructor
             */
            world();

            /**
             * Destructor
             */
            ~world();

            /**
             * @brief Get list of entities
             */
            const entityList &list() const { return _ents; }
//            const entityList &list(const engine::string &type) const
//            {
//                return _entsByType[type];
//            }
//            entityList::const_iterator begin() const { return _ents.begin(); }
//            entityList::const_iterator end() const { return _ents.end(); }

            /**
             * @brief Remove all entities
             */
            void clear();

            /**
             * @brief Reset all entities to original state
             */
            void reset();

//            void operator +=(const entityPtr &ent);
//            entityPtr operator -=(const engine::string &name);
//            entityPtr operator[](const engine::string &name) const;

            /**
             * @brief Add entity into world
             * @param ent pointer for the entity to add in
             */
            void add(const entityPtr &ent);// { return (*this) += ent; }

            /**
             * @brief Remove entity from the world
             * @param name name of the entity to remove
             */
            entityPtr remove(const engine::string &name);// { return (*this) -= name; }

            /**
             * @brief Get pointer for the entity
             * @param name name of need entity
             * @return pointer to the entity with given name
             */
            entityPtr get(const engine::string &name) const;

            /**
             * @brief Rename entity in world
             * @param before name of the entity to rename
             * @param now new entity name
             */
            void rename(const engine::string &before, const engine::string &now);

            /**
             * @brief Used for global application cycle
             */
            void tick(engine::real dt) const;

            // TODO: WTF?
            engine::aabb_t aabb() const;
    };

    /**
     * Shared pointer to the current class
     */
    typedef boost::shared_ptr<world> worldPtr;
}

#endif // GAME_WORLD_HPP_INCLUDED
