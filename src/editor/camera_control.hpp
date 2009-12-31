 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef CAMERA_CONTROL_HPP_INCLUDED
#define CAMERA_CONTROL_HPP_INCLUDED

#include "pre_ogre.hpp"
#include "types.hpp"

namespace engine
{
    namespace input
    {
        class input;
    }
}

namespace editor
{
    class cameraControl
    {
        private:

            engine::quat _yaw, _pitch;

            const engine::input::input &_input;

        public:

            bool movedWithLeftMB;
            engine::real timeLeftToVisibleMouse;

            cameraControl(const engine::input::input &input):
                _yaw(engine::quat::IDENTITY),
                _pitch(engine::quat::IDENTITY),
                _input(input),
                movedWithLeftMB(false),
                timeLeftToVisibleMouse(0)
            {
            }

            void reset();

            void operator()(Ogre::Camera *cam, engine::real dt);
    };
}

#endif // CAMERA_CONTROL_HPP_INCLUDED
