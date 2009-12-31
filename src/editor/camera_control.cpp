 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgreCamera.h>

#include "input/input.hpp"

#include "camera_control.hpp"

#include "game/portal.hpp"

namespace editor
{
    namespace
    {
        float signf(float x)
        {
            return x < 0 ? -1 : 1;
        }
    }

    void cameraControl::operator()(Ogre::Camera *cam, engine::real dt)
    {
        using namespace engine;

        const real MOUSE_VISIBLE_DELAY = 0.5;

        const vec3 POS = cam->getDerivedPosition();
        vec3 pos = POS;
        const quat ROT = cam->getDerivedOrientation();

        const plane XZ(vec3::UNIT_Y, 0);

        const OIS::MouseState &ms = _input.mouse()->getMouseState();

        vec3 rel = _input.mouseRel();

        if (ms.buttonDown(OIS::MB_Right) && ms.buttonDown(OIS::MB_Left))
        {
            vec3 move = ROT.xAxis() * rel.x + vec3(0, -rel.y, 0);
            pos += move * 0.1;

            if (move.squaredLength() > 0.001)
            {
                timeLeftToVisibleMouse = MOUSE_VISIBLE_DELAY;
                movedWithLeftMB = true;
            }
        }
        else if (ms.buttonDown(OIS::MB_Right))
        {
            _yaw = quat(deg(-rel.x), vec3(0, 1, 0)) * _yaw;
            _pitch = _pitch * quat(deg(-rel.y), vec3(1, 0, 0));

            if (rel.x * rel.x + rel.y * rel.y > 0.01)
            {
                timeLeftToVisibleMouse = MOUSE_VISIBLE_DELAY;
            }
        }
        else if (ms.buttonDown(OIS::MB_Left))
        {
            vec3 movement = ROT * vec3(0, 0, rel.y * 0.1);

            // if movement is greater than 0.1cm
            if (movement.squaredLength() > 0.001 * 0.001)
            {
                pos += XZ.projectVector(movement).normalisedCopy() * movement.length();
                movedWithLeftMB = true;
            }
            else
            {
                // didn't move, *might* be picking/clicking
            }
            _yaw = quat(deg(-rel.x), vec3(0, 1, 0)) * _yaw;

            if (movedWithLeftMB || rel.x * rel.x > 0.1)
                timeLeftToVisibleMouse = MOUSE_VISIBLE_DELAY;
        }
        else
        {
            timeLeftToVisibleMouse = 0;
        }

        // check for moving through a portal
        {
            using game::portal;
            BOOST_FOREACH(portal *p, portal::getAllPortals())
            {
                if (!p->parent()) // portal not in a world
                    continue;

                if (p->isWithinFace(POS) && !p->isBehindFace(POS) && p->isBehindFace(pos))
                {
                    mat4 m;
                    m.makeTransform(pos, vec3::UNIT_SCALE, _yaw);

                    m = p->teleportMatrix(m);

                    pos = m.getTrans();
                    _yaw = m.extractQuaternion();

                    break;
                }
            }
        }

        if (_yaw.yAxis().dotProduct(vec3::UNIT_Y) < 0.99999)
        {
            // align Y axis

            quat align = _yaw.yAxis().getRotationTo(vec3::UNIT_Y);
            quat dest = align * _yaw;

            _yaw = quat::Slerp(dt * 3, _yaw, dest, true);
        }

        cam->setPosition(pos);
        cam->setOrientation(_yaw * _pitch);

        timeLeftToVisibleMouse -= dt;
    }

    void cameraControl::reset()
    {
        using engine::quat;
        _yaw = quat::IDENTITY;
        _pitch = quat::IDENTITY;
        movedWithLeftMB = false;
        timeLeftToVisibleMouse = 0;
    }
}
