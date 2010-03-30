 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <Newton.h>

#include <cassert>
#include <algorithm>
#include <boost/foreach.hpp>

#include "world.hpp"
#include "body.hpp"
#include "collision.hpp"
#include "types.hpp"

namespace engine
{
    namespace phys
    {
        namespace
        {
            float saturate(float a)
            {
                return a < 0 ? 0 : (a > 1 ? 1 : a);
            }

            float to01(float a)
            {
                return a * 0.5 + 0.5;
            }
        }

        void contact::_reject()
        {
            if (_valid)
            {
                NewtonContactJointRemoveContact(_joint, _contact);
                _valid = false;
            }
        }

        contact::contact(const NewtonJoint *_joint, void *_contact):
            _valid(true),
            _joint(_joint),
            _contact(_contact),
            _material(NewtonContactGetMaterial(_contact)),
            faceAttrib(NewtonMaterialGetContactFaceAttribute(_material)),
            normalSpeed(NewtonMaterialGetContactNormalSpeed(_material))
        {
            NewtonMaterialGetContactForce(_material, &force.x);
            NewtonMaterialGetContactPositionAndNormal(_material, &position.x, &normal.x);
            NewtonMaterialGetContactTangentDirections(_material, &tangent0.x, &tangent1.x);
        }

        void contact::frictionState(bool use, unsigned tangent) const
        {
            if (_valid)
                NewtonMaterialSetContactFrictionState(_material, use ? 1 : 0, tangent);
        }

        void contact::frictionCoef(float f0, float f1, unsigned tangent) const
        {
            if (_valid)
                NewtonMaterialSetContactFrictionCoef(_material, f0, f1, tangent);
        }

        void contact::softness(float f) const
        {
            if (_valid)
                NewtonMaterialSetContactSoftness(_material, f);
        }

        void contact::elasticity(float f) const
        {
            if (_valid)
                NewtonMaterialSetContactElasticity(_material, f);
        }

        void contact::normalAccel(float f) const
        {
            if (_valid)
                NewtonMaterialSetContactNormalAcceleration(_material, f);
        }

        void contact::normalDir(const vec3 &dir) const
        {
            if (_valid)
                NewtonMaterialSetContactNormalDirection(_material, &dir.x);
        }

        void contact::tangentAccel(float f, unsigned tangent) const
        {
            if (_valid)
                NewtonMaterialSetContactTangentAcceleration(_material, f, tangent);
        }

        void contact::tangentDir(const vec3 &dir) const
        {
            if (_valid)
                NewtonMaterialContactRotateTangentDirections(_material, &dir.x);
        }

        real frac(real f, int *i)
        {
            *i = int(f); // floor int
            return f - real(*i); // fractional part
        }

        void transformCB(const NewtonBody *_body, const float *m1, int threadIndex)
        {
            bodyCast(_body)->_transform(_matrix(m1));
        }

        void forceTorqueCB(const NewtonBody *_body, float dt, int threadIndex)
        {
            bodyCast(_body)->forceTorque(dt);
        }

        body *bodyCast(const NewtonBody *_body)
        {
            body *ret = (body*)NewtonBodyGetUserData(_body);
            assert(ret);
            return ret;
        }

        body::material_t body::material_t::mix(const body::material_t &rhs)
        {
            body::material_t ret;

            ret.elasticity = to01(elasticity + rhs.elasticity);
            ret.softness = std::max(softness, rhs.softness);

            ret.sFric = to01(sFric + rhs.sFric);
            ret.kFric = to01(kFric + rhs.kFric);

            return ret;
        }

        body::body(const world &w):
            _world(w),
            _body(NULL),
            _historyTime(0),
            _recordHistory(true),
            _mass(0),
            _collidable(true),
            _active(true),
            _swept(false),
            _autoSleep(true),
            contactClipPlane(0, 0, 0, 1),
            // default to deferred portals unless we want the instant method
            portalMethod(PORTAL_DEFERRED)
        {
            _freq = w.freq();

            // some fail-safe defaults for the material
            material.softness = 0.05;
            material.kFric = 0;
            material.sFric = 0;
            material.elasticity = -0.25;

            _collision = collisionPtr(new nullCollision(_world));
            _body = NewtonCreateBody(_world, *_collision);
            mass(1);

            NewtonBodySetUserData(_body, this);
            NewtonBodySetTransformCallback(_body, &transformCB);
            NewtonBodySetForceAndTorqueCallback(_body, &forceTorqueCB);

            // no damping
//            float damp[3] = {0};
//            NewtonBodySetAngularDamping(_body, damp);
//            NewtonBodySetLinearDamping(_body, 0);
        }

        body::~body()
        {
            NewtonDestroyBody(_world, _body);
        }

        void body::collision(const collisionPtr &c)
        {
            assert(c);

            _collision = c;
            NewtonBodySetCollision(_body, *c);
            // fix our mass matrix (inertia)
            mass(mass());
            // beta13 tree collision bugfix
            //matrix(matrix());
        }

        void body::scale(const vec3 &s)
        {
            assert(_collision);

            if ((_collision->scale() - s).isZeroLength())
                return;

            // current volume
            float vol = _collision->volume();
            assert(vol > 0.000001 && "volume cannot be 0");
            // current mass
            float m = mass();
            float density = m / vol;

            // scale collision
            _collision->scale(s);

            // get new volume
            vol = _collision->volume();
            // get new mass using
            /*
                density = mass
                          ----
                         volume
                mass = density * volume
            */
            mass(density * vol);

            // wake up everyone inside us
            _world.wake(aabb());
//
//            // add a bit of an impulse and wake us up
//            if (!awake())
//            {
//                wake();
//                impulse(vec3(0, 0.1, 0));
//            }
        }

        void body::_transform(const mat4 &m)
        {
            // warps explicitely handled in matrix()
            if (_recordHistory)
                _addHistory(_historyTime, bodyHistory(m, false));
            else
                transform(m.getTrans(), m.extractQuaternion());
            ///std::cout << "transform " << this << " at " << _historyTime << "\n";
            //transform(m.getTrans(), m.extractQuaternion());
        }

        void body::transform(const vec3 &p, const quat &q)
        {
            // up to the graphical target
        }

        vec3 body::finalGravity() const
        {
            if (!gravity.enabled && !_world.gravity.enabled)
                return vec3(0, 0, 0);

            const gravity_t &g = gravity.enabled ? gravity : _world.gravity;

            vec3 v = g.v;
            if (g.isPoint)
            {
                v -= pos();
                v.normalise();
            }
            return v * g.strength * mass();
        }

        void body::forceTorque(float dt)
        {
            if (_active)
            {
                addForce(finalGravity());

                // terminal velocity
//                vec3 v = vel();
//                if (v.squaredLength() > 400)
//                    vel(v.normalisedCopy() * 20);

                //BOOST_FOREACH(forceTorqueHook &hook, forceTorqueHooks)
                //    hook(*this, dt);

                forceTorqueSignal(*this, dt);
            }
            _historyTime += dt;
        }

        void body::matrix(const mat4 &m0)
        {
            float m1[16];
            _matrix(m0, m1);
            NewtonBodySetMatrix(_body, m1);

            // add new history that is "warped"
            if (_recordHistory)
            {
                if (this->_history.empty())
                    this->_addHistory(_time, bodyHistory(m0, true));
                else
                    this->latestHistory() = bodyHistory(m0, true);
            }
        }

        mat4 body::matrix() const
        {
            float m1[16];
            NewtonBodyGetMatrix(_body, m1);
            return _matrix(m1);
        }

        void body::addForce(const vec3 &f) const
        {
            NewtonBodyAddForce(_body, &f.x);
        }

        void body::addTorque(const vec3 &t) const
        {
            NewtonBodyAddTorque(_body, &t.x);
        }

        void body::mass(float m)
        {
            float i[3], com[3];
            NewtonConvexCollisionCalculateInertialMatrix(*_collision, i, com);
            NewtonBodySetMassMatrix(_body, m, m * i[0], m * i[1], m * i[2]);
            ///NewtonBodySetCentreOfMass(_body, com);
            _mass = m;
        }

        vec3 body::realCOM() const
        {
            float i[3], com[3];
            NewtonConvexCollisionCalculateInertialMatrix(*_collision, i, com);
            return vec3(com[0], com[1], com[2]);
        }

//        float body::mass() const
//        {
//            float m, ix, iy, iz;
//            NewtonBodyGetMassMatrix(_body, &m, &ix, &iy, &iz);
//            return m;
//        }

        void body::vel(const vec3 &v)
        {
            NewtonBodySetVelocity(_body, &v.x);
        }

        vec3 body::vel() const
        {
            vec3 v;
            NewtonBodyGetVelocity(_body, &v.x);
            return v;
        }

        void body::omega(const vec3 &v)
        {
            NewtonBodySetOmega(_body, &v.x);
        }

        vec3 body::omega() const
        {
            vec3 v;
            NewtonBodyGetOmega(_body, &v.x);
            return v;
        }

        void body::pos(const vec3 &p)
        {
            mat4 m = matrix();
            m.setTrans(p);
            matrix(m);
        }

        void body::impulse(const vec3 &v)
        {
            vec3 p(0, 0, 0);
            NewtonBodyAddImpulse(_body, &v.x, &p.x);
        }

        aabb_t body::aabb() const
        {
            vec3 a(0, 0, 0), b(0, 0, 0);

            float mat[16];
            _matrix(matrix(), mat);

            //NewtonBodyGetAABB(_body, &a.x, &b.x);
            NewtonCollisionCalculateAABB(*_collision, mat, &a.x, &b.x);

            return aabb_t(a, b);
        }

        void body::wake()
        {
            //impulse(vec3(0, 0.25, 0));
            NewtonBodySetFreezeState(_body, 0);
        }

        vec3 body::pos() const
        {
            return matrix().getTrans();
        }

        void body::rot(const quat &q)
        {
            mat4 m = matrix();
            m.makeTransform(m.getTrans(), vec3(1, 1, 1), q);
            matrix(m);
        }

        quat body::rot() const
        {
            return matrix().extractQuaternion();
        }

        bool body::awake() const
        {
            return !NewtonBodyGetSleepState(_body);
        }

        void body::lerp(real dt)
        {
            //!
//            const bodyHistory &hist = recordable<bodyHistory>::latestHistory();
//            transform(hist.p, hist.q);
            //return;
            if (!_recordHistory)
                return;

            bodyHistory a, b;

            _time += dt;

            real f = recordable<bodyHistory>::_findHistory(_time - _world.freq(), a, b);

            if (f >= 0)
            {
                if (b.warp)
                {
                    transform(b.p, b.q);

                    // also set our newton body matrix
//                    {
//                        mat4 m0;
//                        m0.makeTransform(b.p, vec3(1, 1, 1), b.q);
//                        float m1[16];
//                        _matrix(m0, m1);
//                        NewtonBodySetMatrix(_body, m1);
//                    }
                }
                else
                {
                    vec3 p = a.p + (b.p - a.p) * f;
                    quat q = quat::Slerp(f, a.q, b.q, true);
                    transform(p, q);

                    // also set our newton body matrix
//                    {
//                        mat4 m0;
//                        m0.makeTransform(p, vec3(1, 1, 1), q);
//                        float m1[16];
//                        _matrix(m0, m1);
//                        NewtonBodySetMatrix(_body, m1);
//                    }
                }
            }
        }

        bool body::aabbOverlap(body &o)
        {
            BOOST_FOREACH(body *i, ignoreList)
                if (&o == i)
                    return false;

            // don't check for _collidable here because then
            // for example triggers won't be able to check for collisions
            return _active;
//            return _collidable && _active;
        }

        void body::processContacts(body &o,
            const contactList &contacts, contactList &rejections, float dt)
        {
            if (!_collidable)
            {
                // reject everything
                rejections = contacts;
                return;
            }

            //rejections.push_back(contacts.front());
            vec3 n(contactClipPlane.x, contactClipPlane.y, contactClipPlane.z);
            if (n.dotProduct(n) < 0.001) // don't clip
                return;

            contactList::const_iterator i = contacts.begin();
            for (; i != contacts.end(); ++i)
            {
                contact &c = *(*i); // it's a shared pointer
                if (n.dotProduct(c.position) + contactClipPlane.w < 0)
                    rejections.push_back(*i);
            }
        }

        void body::contactsCriticalSection()
        {
        }

        void body::autoSleep(bool b)
        {
            NewtonBodySetAutoSleep(_body, b);
            _autoSleep = b;
        }

        void body::swept(bool b)
        {
            NewtonBodySetContinuousCollisionMode(_body, b);
            _swept = b;
        }

        void body::active(bool b)
        {
            _active = b;
            //collidable(_active);

            if (!_active)
                NewtonBodySetFreezeState(_body, 1);
            else
                wake();

//            if (!_active)
//            {
//                // set 0 mass to not move anywhere
//                NewtonBodySetMassMatrix(_body, 0, 0, 0, 0);
//            }
//            else
//            {
//                // reset mass
//                mass(mass());
//            }
        }
    }
}
