 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <algorithm>

#include <OgreSceneNode.h>
#include <OgreCamera.h>

#include <Newton.h>

#include "phys/world.hpp"
#include "phys/body.hpp"
#include "character.hpp"
#include "data_dir.hpp"
#include "safe_any.hpp"

#include "entity_types.hpp"

namespace game
{
    const interfaceType character::TYPE = ENTITY_CHARACTER;

    character::character(
        const engine::string &name,
        const genericProp::constructParams &params):
        genericProp(name, params),
        _upJoint(NULL),
        _pitchNode(NULL),
        _floor(NULL),
        _yawAccum(0),
        movement(0, 0, 0),
        speed(1)
    {
        _interfaces.push_back(character::TYPE);

        // Characters should be fast and easy, so use the instant teleport method
//        portalMethod = PORTAL_INSTANT;

        mass(10);

        autoSleep(false); // Never let the character fall asleep

        _pitchNode = _node->createChildSceneNode();

        using namespace engine;
        vec3 pin = up();
        _upJoint = NewtonConstraintCreateUpVector(_world, &pin.x, _body);
    }

//    character::character(
//        const engine::string &name, const character &o):
//        genericProp(name, o),
//        _pitchNode(NULL),
//        _headNode(NULL),
//        _floor(NULL),
//        movement(0, 0, 0),
//        speed(1)
//    {
//        _interfaces.push_back(character::TYPE);
//
//        _pitchNode = _node->createChildSceneNode();
//        _headNode = _pitchNode->createChildSceneNode();
//        _headNode->translate(engine::vec3(0, 0.35, 0));
//    }

    character::~character()
    {
        NewtonDestroyJoint(_world, _upJoint);
    }

    void character::forceTorque(float dt)
    {
        using namespace engine;
        using namespace engine::phys;

        genericProp::forceTorque(dt);

        quat rotQ = rot();

        omega(rotQ * vec3(0, _yawAccum, 0));
        _yawAccum = 0;

        if (_floor)
        {
//            // If we're not moving, simply set the velocity to the floor velocity
//            if (movement.isZeroLength())
//            {
//                vel(_floor->vel());
//            }
//            // Otherwise use forces to move us
//            else
            {
                body::material_t mat = material.mix(_floor->material);

                vec3 upV = rotQ.yAxis();

//                Orthogonalization should do the trick:
//
//                forward' = forward - up * dot(forward,up)/dot(up,up);

                vec3 finalMove = movement * scale();
                finalMove = rotQ * finalMove * speed - vel();
                if (_floor->mass() > 1)
                    finalMove += _floor->vel();
                //finalMove = finalMove - upV * finalMove.dotProduct(upV) / upV.dotProduct(upV);

                float coef = 0.005 * (1.0 - mat.kFric) + 0.7 * mat.kFric;
                addForce(finalMove * mass() * coef / dt);
            }
        }

        _floor = NULL;

//        omega(vec3::ZERO);

//        phys::body::rot(_node->_getDerivedOrientation());
    }

//    entityPtr character::clone(const engine::string &name)
//    {
//        characterPtr p(new character(name, *this));
//        return p;
//    }

    engine::vec3 character::up() const
    {
        return rot().yAxis();
    }

    void character::processContacts(engine::phys::body &o,
        const engine::phys::contactList &contacts, engine::phys::contactList &rejections, float dt)
    {
        using namespace engine;
        using namespace engine::phys;

        genericProp::processContacts(o, contacts, rejections, dt);

        vec3 upV = up();

        contactList::const_iterator i = contacts.begin();
        for (; i != contacts.end(); ++i)
        {
            contact &c = *(*i); // It's a shared pointer

//            vec3 normal = _node->_getDerivedPosition() - c.position;
//            normal.normalise();
            vec3 normal = c.normal;

            if (normal.dotProduct(upV) > 0.707)
            {
                _floor = &o;
            }
            // testing wall walking
//            else if (o.mass() < 0.01)
//            {
//                gravity = -normal * 9.81;
//            }

            c.frictionState(false, 0);
            c.frictionState(false, 1);
            c.frictionCoef(0, 0, 0);
            c.frictionCoef(0, 0, 1);
            c.elasticity(0);
        }
    }

    void character::yaw(float f)
    {
        using namespace engine;
        _yawAccum += f;
//        _node->yaw(deg(f));
    }

    void character::pitch(float f)
    {
        _pitchNode->pitch(engine::deg(f));
    }

    void character::jump()
    {
        using namespace engine;
        if (_floor)
        {
            vel(vel() + up() * 5);
            _floor = NULL;
        }
    }

    void character::tick(engine::real dt)
    {
        genericProp::tick(dt);

        using namespace engine;

        // align us with gravity
        vec3 destUp = -finalGravity().normalisedCopy();

        quat currentRot = rot();
        vec3 currentUp = currentRot.yAxis();
        if (currentUp.dotProduct(destUp) < 0.9999)
        {
            quat align = currentUp.getRotationTo(destUp);
            vec3 axis;
            deg ang;
            align.ToAngleAxis(ang, axis);

            //std::cout << "axis: " << axis << "\n";
//            quat dest = align * currentRot;

//            currentRot = quat::nlerp(dt * 2, currentRot, dest, true);
            currentRot = quat(deg(dt * 180), axis) * currentRot;

//            _node->setOrientation(currentRot);
            NewtonDestroyJoint(_world, _upJoint);
            rot(currentRot);

            vec3 pin = currentRot.yAxis();
            //NewtonUpVectorSetPin(_upJoint, &pin.x);
            _upJoint = NewtonConstraintCreateUpVector(_world, &pin.x, _body);
//            phys::body::rot(currentRot);
        }

        movement = vec3(0, 0, 0);
    }

    void character::transform(const engine::vec3 &p, const engine::quat &q)
    {
        genericProp::transform(p, q);
        // only transform the position, not the rotation
//        _node->setPosition(p);
    }

    entityPtr characterFactory::create(const engine::string &name) const
    {
        using namespace engine;

        characterPtr ent(new character(name, _params));

        return ent;
    }
}
