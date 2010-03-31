 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

/**
 * @namespace editor
 * @brief Consist of tools for working with objects in editor mode
 */
namespace editor
{
	/**
	 * @class cameraControl
	 * @brief Represents editor camera moving interface
	 * @see class Ogre::Camera
	 */
    class cameraControl
    {
        private:
			/// Quaternion for yawing and pitching the camera
            engine::quat _yaw, _pitch;

            const engine::input::input &_input;

        public:
            /// Flag that camera was moved with left mouse button
            bool movedWithLeftMB;

            // TODO: Wtf
            engine::real timeLeftToVisibleMouse;

            /**
             * Constructor
             */
            cameraControl(const engine::input::input &input):
                _yaw(engine::quat::IDENTITY),
                _pitch(engine::quat::IDENTITY),
                _input(input),
                movedWithLeftMB(false),
                timeLeftToVisibleMouse(0)
            {
            }

            /**
             * @brief Reset camera
             */
            void reset();

            void operator()(Ogre::Camera *cam, engine::real dt);
    };
}

#endif // CAMERA_CONTROL_HPP_INCLUDED
