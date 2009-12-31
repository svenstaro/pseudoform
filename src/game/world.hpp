 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

    class world
    {
        private:

            entityList _ents;
//            entityListByType _entsByType;

        public:

            world();
            ~world();

            const entityList &list() const { return _ents; }
//            const entityList &list(const engine::string &type) const
//            {
//                return _entsByType[type];
//            }
//            entityList::const_iterator begin() const { return _ents.begin(); }
//            entityList::const_iterator end() const { return _ents.end(); }

            void clear(); // clear all entities
            void reset(); // reset all entities to original state

//            void operator +=(const entityPtr &ent);
//            entityPtr operator -=(const engine::string &name);
//            entityPtr operator[](const engine::string &name) const;

            // utilities
            void add(const entityPtr &ent);// { return (*this) += ent; }
            entityPtr remove(const engine::string &name);// { return (*this) -= name; }
            entityPtr get(const engine::string &name) const;
            void rename(const engine::string &before, const engine::string &now);

            void tick(engine::real dt) const;

            engine::aabb_t aabb() const;
    };

    typedef boost::shared_ptr<world> worldPtr;

}

#endif // GAME_WORLD_HPP_INCLUDED
