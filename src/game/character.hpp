 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef CHARACTER_HPP_INCLUDED
#define CHARACTER_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include "pre_ogre.hpp"
#include "generic_prop.hpp"

#include "phys/body.hpp"

struct NewtonJoint;

namespace game
{
	/**
	 * @class character
	 * @brief Represents character in game world
	 * @see class genericProp, class entity
	 * @remarks We should use this class for making another
	 * characters for the game like main player, monsters, ...
	 */
    class character: public genericProp
    {
        protected:
			// TODO: upJoint in character class
            NewtonJoint *_upJoint;
            /// Characted node
            Ogre::SceneNode *_pitchNode;
            /// Character physics body
            engine::phys::body *_floor;
            // TODO yawAccum in character class
            float _yawAccum;

        public:
            static const interfaceType TYPE;

            /// Character movement vector
            engine::vec3 movement;
            /// Character moving speed
            float speed;

            /**
             * Constructor
             */
            character(const engine::string &name, const genericProp::constructParams &params);
            /**
             * Destructor
             */
            virtual ~character();

            /**
             * @brief Pitch character
             */
            virtual void pitch(float f);
            /**
             * @brief Yaw character
             */
            virtual void yaw(float f);

            /**
             * @brief Make characted jumping
             */
            virtual void jump();

            /**
             * @brief Get upper vector
             * @return vector3, where is the top
             */
            virtual engine::vec3 up() const;

            /**
             * @brief Process contacts between character and another bodies
             */
            virtual void processContacts(engine::phys::body &o,
                const engine::phys::contactList &contacts,
                engine::phys::contactList &rejections, float dt);

            virtual void tick(engine::real dt);

            /**
             * @brief Apply torque to the character
             */
            virtual void forceTorque(float dt);

            /**
             * @brief Transform character position
             * @param p additional vector
             * @param q quaternion for rotating
             */
            virtual void transform(const engine::vec3 &p, const engine::quat &q);

//            virtual entityPtr clone(const engine::string &name);
    };

    typedef boost::shared_ptr<character> characterPtr;

    /**
     * @class characterFactory
     * @see class entityFactory, class entity
     * @remarks for more information
     * visit: http://en.wikipedia.org/wiki/Factory_method_pattern
     */
    class characterFactory: public entityFactory
    {
        private:
            genericProp::constructParams _params;

        public:
            /**
             * Constructor
             */
            characterFactory(const genericProp::constructParams &params):
                entityFactory(character::TYPE),
                _params(params)
            {
            }

            /**
             * @brief Make new character
             * @param name of new character
             */
            entityPtr create(const engine::string &name) const;
    };

    /**
     * Shared pointer to the current class
     */
    typedef boost::shared_ptr<characterFactory> characterFactoryPtr;
}

#endif // CHARACTER_HPP_INCLUDED
