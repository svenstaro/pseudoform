 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

    class character: public genericProp
    {
        protected:

            NewtonJoint *_upJoint;

            Ogre::SceneNode *_pitchNode;

            engine::phys::body *_floor;

            float _yawAccum;

        public:

            static const interfaceType TYPE;

            engine::vec3 movement;
            float speed;

            character(const engine::string &name, const genericProp::constructParams &params);
            virtual ~character();

            virtual void pitch(float f);
            virtual void yaw(float f);

            virtual void jump();

            virtual engine::vec3 up() const;

            virtual void processContacts(engine::phys::body &o,
                const engine::phys::contactList &contacts,
                engine::phys::contactList &rejections, float dt);

            virtual void tick(engine::real dt);

            virtual void forceTorque(float dt);

            virtual void transform(const engine::vec3 &p, const engine::quat &q);

//            virtual entityPtr clone(const engine::string &name);
    };

    typedef boost::shared_ptr<character> characterPtr;

    class characterFactory: public entityFactory
    {
        private:

            genericProp::constructParams _params;

        public:

            characterFactory(const genericProp::constructParams &params):
                entityFactory(character::TYPE),
                _params(params)
            {
            }

            entityPtr create(const engine::string &name) const;
    };

    typedef boost::shared_ptr<characterFactory> characterFactoryPtr;

}


#endif // CHARACTER_HPP_INCLUDED
