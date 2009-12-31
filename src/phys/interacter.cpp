 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "interacter.hpp"
#include "body.hpp"
#include "ray.hpp"

namespace engine
{
    namespace phys
    {

        interacter::interacter():
            _body(NULL),
            _localPos(0, 0, 0),
            _pos(0, 0, 0),
            _dir(0, 0, -1),
            _dist(0)
        {
        }

        interacter::~interacter()
        {
            if (_hookConnection.connected())
                _hookConnection.disconnect();
        }

        void interacter::enable(const world &w)
        {
            if (_hookConnection.connected())
                _hookConnection.disconnect();

            const float DIST = 100;
            ray::hitList hits = ray()(w, _pos, _pos + _dir * DIST);
            if (hits.empty())
                return;

            // we want first hit
            const phys::ray::hitInfo &hit = hits.front();

            _body = hit.b;
            _localPos = _body->matrix().inverse() * hit.p;
            _dist = DIST * hit.d; // hit.d is [0..1] normalised

            _hookConnection = _body->forceTorqueSignal.connect(
                boost::bind(&interacter::_hook, this, _1));
        }
        void interacter::disable()
        {
            if (_hookConnection.connected())
                _hookConnection.disconnect();
            _body = NULL;
        }

        void interacter::_hook(body &b)
        {
            vec3 destPos = _pos + _dir * _dist;
            vec3 currentPos = _body->matrix() * _localPos;

            vec3 force = (destPos - currentPos) * _body->mass() * 100;
            vec3 dampForce = _body->vel() * _body->mass() * 10;

            b.addForce(force -= dampForce);

            vec3 torque = (currentPos - _body->pos()).crossProduct(force);
            vec3 dampTorque = _body->omega() * _body->mass() * 0.1;

            b.addTorque(torque -= dampTorque);
        }

    }
}
