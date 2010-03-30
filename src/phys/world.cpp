 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <Newton.h>

#include <cmath>
#include <cassert>
#include <algorithm>

#include <boost/foreach.hpp>

#include "world.hpp"
#include "body.hpp"

namespace engine
{
    namespace phys
    {
        int beginContactCB(const NewtonMaterial *m, const NewtonBody *b0, const NewtonBody *b1,
            int thread)
        {
            body *body0 = bodyCast(b0);
            body *body1 = bodyCast(b1);

            bool ret0 = body0->aabbOverlap(*body1);
            bool ret1 = body1->aabbOverlap(*body0);

            return ret0 && ret1;
        }

        void processContactCB(const NewtonJoint *_joint, float dt, int thread)
        {

            NewtonWorld *_world = NewtonBodyGetWorld(NewtonJointGetBody0(_joint));

            body *body0 = bodyCast(NewtonJointGetBody0(_joint));
            body *body1 = bodyCast(NewtonJointGetBody1(_joint));

            //const size_t NUM_CONTACTS = NewtonContactJointGetContactCount(_joint);
            contactList contacts, rejections;

            const unsigned NUM_CONTACTS = NewtonContactJointGetContactCount(_joint);

            void *_contact = NewtonContactJointGetFirstContact(_joint);
            // iterate over each contact, set its defaults, and add it
            while (_contact)
            {

                contactPtr c(new contact(_joint, _contact));

                // set the default material properties

                body::material_t mat = body0->material.mix(body1->material);

                c->elasticity(mat.elasticity);
                c->softness(mat.softness);
                c->frictionCoef(mat.sFric, mat.kFric, 0);
                c->frictionCoef(mat.sFric, mat.kFric, 1);

                c->frictionState(true, 0);
                c->frictionState(true, 1);

                contacts.push_back(c);

                _contact = NewtonContactJointGetNextContact(_joint, _contact);
            }

//            if (NUM_CONTACTS != contacts.size())
//            {
//            std::cerr << "NUM_CONTACTS " << NUM_CONTACTS << "\n";
//            std::cerr << "contacts.size() " << contacts.size() << "\n";
//            }
            assert(NUM_CONTACTS == contacts.size());

            body0->processContacts(*body1, contacts, rejections, dt);
            body1->processContacts(*body0, contacts, rejections, dt);

            // Do all rejections here
            NewtonWorldCriticalSectionLock(_world);
            {
                // Reject all rejected contacts
                // (they have an auto-check to not reject twice)
                BOOST_FOREACH(contactPtr &c, rejections)
                {
                    c->_reject();
                }

                body0->contactsCriticalSection();
                body1->contactsCriticalSection();
            }
            NewtonWorldCriticalSectionUnlock(_world);
        }

//        void endContactCB(const NewtonMaterial *m, const NewtonBody *b0, const NewtonBody *b1,
//            int thread)
//        {
////            body *body0 = (body*)NewtonBodyGetUserData(b0);
////            body *body1 = (body*)NewtonBodyGetUserData(b1);
////
////            body0->endContact(body1);
////            body1->endContact(body0);
//        }

        void world::clearCache() const
        {
            NewtonInvalidateCache(_world);
        }

        void world::threads(unsigned n)
        {
            NewtonSetThreadsCount(_world, n);
        }

        void world::solver(unsigned n)
        {
            NewtonSetSolverModel(_world, n);
        }

        void world::friction(unsigned n)
        {
            NewtonSetFrictionModel(_world, n);
        }

        void world::platform(unsigned n)
        {
            NewtonSetPlatformArchitecture(_world, n);
        }

        world::world():
            _world(NULL),
            _accum(0),
            _freq(1.0 / 60.0)
        {
            // default to global gravity
            gravity.enabled = true;

            _world = NewtonCreate();
            const float WORLD_SIZE = 1000;
            const vec3 A(-WORLD_SIZE, -WORLD_SIZE, -WORLD_SIZE), B(-A);
            NewtonSetWorldSize(_world, &A.x, &B.x);

            platform(PLAT_OPTMIZED);
            solver(3);
            friction(FRIC_ADAPTIVE);
            threads(2);
            clearCache();

            int mid = NewtonMaterialGetDefaultGroupID(_world);
            // allow bodies to be swept-tested
            NewtonMaterialSetContinuousCollisionMode(_world, mid, mid, 1);
            NewtonMaterialSetCollisionCallback(_world, mid, mid, NULL,
                &beginContactCB, &processContactCB);
        }

        world::~world()
        {
            NewtonDestroy(_world);
        }

        void world::tick(real dt)
        {
            const real MIN_FPS = 10.0;
            if (dt > 1.0 / MIN_FPS)
                dt = 1.0 / MIN_FPS;

            _lastStep = 0;
            for (_accum += dt; _accum >= _freq; _accum -= _freq, _lastStep += _freq)
                NewtonUpdate(_world, _freq);

            // Iterate over each body and lerp it
            NewtonBody *_body = NewtonWorldGetFirstBody(_world);
            while (_body)
            {
                bodyCast(_body)->lerp(dt);
                _body = NewtonWorldGetNextBody(_world, _body);
            }
        }

        void world::freq(real f)
        {
            _freq = f;

            // iterate over each body and tell it the new frequency (recordable::_freq)
//            NewtonBody *_body = NewtonWorldGetFirstBody(_world);
//            while (_body)
//            {
//                bodyCast(_body)->freq(_freq);
//                _body = NewtonWorldGetNextBody(_world, _body);
//            }
        }

        void _wakeAABBCB(const NewtonBody *_body, void* userData)
        {
            bodyCast(_body)->wake();
        }

        void world::wake(const aabb_t &aabb) const
        {
            vec3 min = aabb.getMinimum(), max = aabb.getMaximum();
            NewtonWorldForEachBodyInAABBDo(_world, &min.x, &max.x, &_wakeAABBCB, NULL);
        }

    }
}
