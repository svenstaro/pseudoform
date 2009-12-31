 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef INPUT_HPP_INCLUDED
#define INPUT_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include "types.hpp"

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

        enum modifier
        {
            M_ALT, M_CTRL, M_SHIFT
        };

        const unsigned LAST_KEY = OIS::KC_MEDIASELECT;// must be eventually changed!
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

        inline OIS::MouseButtonID convert(mouseButton b)
        {
            return OIS::MouseButtonID(unsigned(b) - LAST_KEY - 1);
        }

        inline mouseButton convert(OIS::MouseButtonID b)
        {
            return mouseButton(unsigned(b) + LAST_KEY + 1);
        }

        class input
        {
            private:

                OIS::InputManager *_mgr;
                OIS::Keyboard *_keys;
                OIS::Mouse *_mouse;

            public:

                OIS::InputManager *mgr() const { return _mgr; }
                OIS::Keyboard *keys() const { return _keys; }
                OIS::Mouse *mouse() const { return _mouse; }

                bool isHeld(unsigned c) const;

                bool mod(modifier m) const;

                input(size_t hwnd);
                ~input();

                vec3 mouseRel() const;
                mutable float mouseSens;

                void resize(unsigned w, unsigned h) const;

                void capture() const;
                void operator()() const { capture(); }

                void grab(OIS::KeyListener *k = NULL, OIS::MouseListener *m = NULL) const;
                void operator()(OIS::KeyListener *k, OIS::MouseListener *m) const
                {
                    grab(k, m);
                }
        };

        typedef boost::shared_ptr<input> inputPtr;

    }

}

#endif // INPUT_HPP_INCLUDED
