 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef BODY_HPP_INCLUDED
#define BODY_HPP_INCLUDED

#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/optional.hpp>
#include <boost/signal.hpp>

#include "types.hpp"
#include "recordable.hpp"
#include "collision.hpp"
#include "gravity.hpp"

struct NewtonBody;
struct NewtonCollision;
struct NewtonMaterial;
struct NewtonJoint;

namespace engine
{
    namespace phys
    {
        class world;
        class contact;
        class body;

        typedef boost::shared_ptr<contact> contactPtr;
        typedef std::list<contactPtr> contactList;

        typedef boost::signal<void(body&, float)> forceTorqueSignal_t;

        struct bodyHistory
        {
            // 3 + 4 = 7 floats
            // * 4 bytes = 24 bytes per history entry
            vec3 p;
            quat q;
            bool warp;

            bodyHistory(bool w = false):
                p(vec3::ZERO), q(quat::IDENTITY), warp(w) {}

            bodyHistory(const vec3 &p, const quat &q, bool w = false):
                p(p), q(q), warp(w) {}

            bodyHistory(const mat4 &m, bool w = false):
                p(m.getTrans()), q(m.extractQuaternion()), warp(w) {}

            bodyHistory(const bodyHistory &rhs):
                p(rhs.p), q(rhs.q), warp(rhs.warp) {}
        };

        class body: public recordable<bodyHistory>
        {
            protected:

                const world &_world;
                NewtonBody *_body;

                // this is the variable we use to add history
                // it's incremented on every force and torque call
                // this is "real" time
                real _historyTime;

                bool _recordHistory;

                float _mass;
                bool _collidable, _active, _swept, _autoSleep;

                collisionPtr _collision;

            public:

                operator NewtonBody*() const { return _body; }

                gravity_t gravity;

                virtual void clearHistory()
                {
                    _history.clear();
                    //_history[0] = bodyHistory();

                    _time = 0;
                    _historyTime = 0;
                }

                forceTorqueSignal_t forceTorqueSignal;

                boost::any misc;

                struct material_t
                {
                    float sFric, kFric;
                    float elasticity;
                    float softness;

                    material_t mix(const material_t &rhs);
                } material;

                std::list<body*> ignoreList; // AABB collisions rejected in this list

                // clips all collisions behind this plane (if not a 0/null plane)
                vec4 contactClipPlane;

                // tells the portals which method to use for transportation
                enum portalMethod_t
                {
                    PORTAL_INSTANT, // old school, fast and easy
                    PORTAL_DEFERRED // new school, 100% seamless, stable, accurate, etc.
                } portalMethod;

                body(const world &w);
                virtual ~body();

                collisionPtr collision() const { return _collision; }
                void collision(const collisionPtr &c);
                void scale(const vec3 &s);
                vec3 scale() const { return _collision->scale(); }

                // the real newton callback for a transformation
                virtual void _transform(const mat4 &m);

                // graphical interfacing "callback"
                virtual void transform(const vec3 &p, const quat &q);

                // callback for any forces and torques
                virtual void forceTorque(float dt);

                // implement interpolation
                virtual void lerp(real dt);

                virtual bool aabbOverlap(body &o);
                // process each contact against another body
                virtual void processContacts(body &o,
                    const contactList &contacts, contactList &rejections, float dt);
                // this is called during the critical section lock
                // (also when contacts are actually rejected)
                virtual void contactsCriticalSection();

                mat4 matrix() const;
                virtual void matrix(const mat4 &m);

                void mass(float m);
                float mass() const { return _mass; }

                vec3 vel() const;
                virtual void vel(const vec3 &v);

                vec3 omega() const;
                virtual void omega(const vec3 &v);

                vec3 pos() const;
                virtual void pos(const vec3 &v);

                quat rot() const;
                virtual void rot(const quat &q);

                vec3 realCOM() const; // "real" calculated center of mass

                virtual void impulse(const vec3 &v);

                bool awake() const;
                void wake();
                void autoSleep(bool b);
                bool autoSleep() const { return _autoSleep; }
                void swept(bool b);
                bool swept() const { return _swept; }

                void addForce(const vec3 &f) const;
                void addTorque(const vec3 &t) const;

                vec3 finalGravity() const;

                bool collidable() const { return _collidable; }
                void collidable(bool b) { _collidable = b; }

                void active(bool b);
                bool active() const { return _active; }

                aabb_t aabb() const;
        };

        body *bodyCast(const NewtonBody *_body);

        class contact
        {
            private:

                bool _valid;

                const NewtonJoint *_joint;
                void *_contact;
                const NewtonMaterial *_material;

            public:

                contact(const NewtonJoint *_joint, void *_contact);

                unsigned faceAttrib;

                float normalSpeed;
                vec3 force;
                vec3 position;
                vec3 normal;
                vec3 tangent0;
                vec3 tangent1;

                void frictionState(bool use, unsigned tangent) const;
                void frictionCoef(float f0, float f1, unsigned tangent) const;
                void softness(float f) const;
                void elasticity(float f) const;
                void normalAccel(float f) const;
                void normalDir(const vec3 &dir) const;
                void tangentAccel(float f, unsigned tangent) const;
                void tangentDir(const vec3 &dir) const;

                void _reject();

        };
    }
}

#endif // BODY_HPP_INCLUDED
