 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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
	/**
	 * @class trigger
	 * @see class genericProp
	 */
    class trigger: public genericProp
    {
        private:
			/**
			 * @structure touchInfo
			 * @brief Consist of information about
			 * touching trigger area
			 */
            struct touchInfo
            {
            	/// Entity which touched the trigger
                entityPtr ent;
                /// Was the entity auto sleep?
                bool wasAutoSleep;
                /// Time since entity contacted with trigger
                float timeSinceContact;
            };

            typedef std::map<engine::phys::body*, touchInfo> touchInfoList;
            /// List of tocuhes info
            touchInfoList _touches;

            typedef std::list<entityPtr> entityList;
            /// List of entities which were entered to the trigger
            entityList _entered;

            /// Trigger own script
            const engine::script::script &_script;

            /**
             * @brief Trigger callback
             */
            void _callback(const engine::string &str, const entityPtr &ent);

        public:
            static const interfaceType TYPE;

            /// TODO
            engine::string onEnter, onExit;

            /**
             * Constructor
             */
            trigger(
                const engine::string &name,
                const genericProp::constructParams &params,
                const engine::script::script &script);

            /**
             * Destructor
             */
            virtual ~trigger();

            /**
             * @brief Process trigger contact
             */
            virtual void processContacts(engine::phys::body &o,
                const engine::phys::contactList &contacts,
                engine::phys::contactList &rejections, float dt);

            virtual void tick(engine::real dt);

            virtual void load(const levelFormat::entity &desc);
            virtual void save(levelFormat::entity &desc);

            virtual void reset();
    };

    typedef boost::shared_ptr<trigger> triggerPtr;

    /**
     * @class triggerFactory
     * @see class trigger, class entity, class entityFactory
     * @remarks for more information
     * visit: http://en.wikipedia.org/wiki/Factory_method_pattern
     */
    class triggerFactory: public entityFactory
    {
        private:
            genericProp::constructParams _params;
            const engine::script::script &_script;

        public:
            /**
             * Constructor
             */
            triggerFactory(const genericProp::constructParams &params,
                const engine::script::script &script):
                entityFactory(trigger::TYPE),
                _params(params),
                _script(script)
            {
            }

            /**
             * @brief create trigger for the factory
             * @param name new trigger name
             */
            entityPtr create(const engine::string &name) const;
    };

    /**
     * Shared pointer to the current class
     */
    typedef boost::shared_ptr<triggerFactory> triggerFactoryPtr;
}

#endif // TRIGGER_HPP_INCLUDED
