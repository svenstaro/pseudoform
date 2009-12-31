 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef TRIGGER_HPP_INCLUDED
#define TRIGGER_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include <list>

#include "script/script.hpp"
#include "generic_prop.hpp"

namespace game
{

    class trigger: public genericProp
    {
        private:

            struct touchInfo
            {
                entityPtr ent;
                bool wasAutoSleep;
                float timeSinceContact;
            };

            typedef std::map<engine::phys::body*, touchInfo> touchInfoList;

            touchInfoList _touches;

            typedef std::list<entityPtr> entityList;
            entityList _entered;

            const engine::script::script &_script;

            void _callback(const engine::string &str, const entityPtr &ent);

        public:

            static const interfaceType TYPE;

            engine::string onEnter, onExit;

            trigger(
                const engine::string &name,
                const genericProp::constructParams &params,
                const engine::script::script &script);
            virtual ~trigger();

            virtual void processContacts(engine::phys::body &o,
                const engine::phys::contactList &contacts,
                engine::phys::contactList &rejections, float dt);

            virtual void tick(engine::real dt);

            virtual void load(const levelFormat::entity &desc);
            virtual void save(levelFormat::entity &desc);

            virtual void reset();
    };

    typedef boost::shared_ptr<trigger> triggerPtr;

    class triggerFactory: public entityFactory
    {
        private:

            genericProp::constructParams _params;
            const engine::script::script &_script;

        public:

            triggerFactory(const genericProp::constructParams &params,
                const engine::script::script &script):
                entityFactory(trigger::TYPE),
                _params(params),
                _script(script)
            {
            }

            entityPtr create(const engine::string &name) const;
    };

    typedef boost::shared_ptr<triggerFactory> triggerFactoryPtr;

}

#endif // TRIGGER_HPP_INCLUDED
