 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef INPUT_HPP_INCLUDED
#define INPUT_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include "types.hpp"

/**
 * @namespace OIS
 * @brief Handle all need OIS-input devices
 */
namespace OIS
{
    class InputManager;
    class Keyboard;
    class Mouse;
    class KeyListener;
    class MouseListener;
}

namespace engine
{
    namespace input
    {
		/**
		 * @enum modifier
		 * @brief Keys additional modificators
		 */
        enum modifier
        {
            M_ALT, M_CTRL, M_SHIFT
        };

        /**
         * TODO: Comment this
         */
        const unsigned LAST_KEY = OIS::KC_MEDIASELECT; // Must be eventually changed!

        /**
         * @enum mouseButton
         * @brief predefined ids of mouse buttons
         */
        enum mouseButton
        {
            MB_0 = LAST_KEY + 1, // left
            MB_1, // right
            MB_2, // middle
            MB_3, // ext left
            MB_4, // ext right
            MB_5, // etc.
            MB_6, // etc.
            MB_7, // etc.
            NUM_INPUT_VALUES
        };

        /**
         * @brief Converts mouseButton structure object into OIS
         * @param b object of mouseButton structure
         * @return OIS native structure referred to the mouse
         */
        inline OIS::MouseButtonID convert(mouseButton b)
        {
            return OIS::MouseButtonID(unsigned(b) - LAST_KEY - 1);
        }

        /**
         * @brief Converts OIS mouse structure object into mouseButton one
         * @param b object of OIS mouse structure
         * @return mouseButton structure, our own mouse structure
         */
        inline mouseButton convert(OIS::MouseButtonID b)
        {
            return mouseButton(unsigned(b) + LAST_KEY + 1);
        }

		/**
		 * @class input
		 * @brief Manages input system
		 */
        class input
        {
            private:
				/// Input manager handle
                OIS::InputManager *_mgr;

                /// Input keyboard handle
                OIS::Keyboard *_keys;

                /// Input mouse handle
                OIS::Mouse *_mouse;

            public:
                /**
                 * @brief Get input manager handle
                 */
                OIS::InputManager *mgr() const { return _mgr; }

                /**
                 * @brief Get keyboard handle
                 */
                OIS::Keyboard *keys() const { return _keys; }

                /**
                 * @brief Get mouse handle
                 */
                OIS::Mouse *mouse() const { return _mouse; }

                /**
                 * @brief Is given key helded (pushed)
                 *
                 * Provides information about state of given key code
                 * @param c key code
                 * @return true if given key is helded
                 */
                bool isHeld(unsigned c) const;

                /**
                 * @brief Is given modifier pushed
                 *
                 * Provides information about key modifiers state
                 * @param m modifier name
                 * @return true if modifier is active
                 */
                bool mod(modifier m) const;

                /**
                 * Constructor
                 */
                input(size_t hwnd);
                /**
                 * Destructor
                 */
                ~input();

                /**
                 * @brief Get mouse relative position
                 *
                 * This method scan mouse state for the x,y,z and makes vector
                 * @return vec3 built vector from mouse position coordinates
                 */
                vec3 mouseRel() const;

                /// Sensitivity of mouse cursor
                mutable float mouseSens;

                /**
                 * @brief Get mouse position due to given dimensions
                 * @param w new width size of area to use
                 * @param h new height size of area to use
                 */
                void resize(unsigned w, unsigned h) const;

                /**
                 * @brief Capture current mouse and keyboards state
                 */
                void capture() const;
                void operator()() const { capture(); }

                /**
                 * @brief Set events callback of given listeners
                 * @param k keys listener to set callback for
                 * @param m mouse listener to set callback for
                 */
                void grab(OIS::KeyListener *k = NULL, OIS::MouseListener *m = NULL) const;
                void operator()(OIS::KeyListener *k, OIS::MouseListener *m) const
                {
                    grab(k, m);
                }
        };

        /**
         * Shared pointer to the current class
         */
        typedef boost::shared_ptr<input> inputPtr;
    }
}

#endif // INPUT_HPP_INCLUDED
