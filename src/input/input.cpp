 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <cstdlib>

#include <fstream>

#include <OIS/OIS.h>

#include "input.hpp"
#include "keys.hpp"
#include "log.hpp"
#include "data_dir.hpp"

namespace engine
{
    namespace input
    {

        vec3 input::mouseRel() const
        {
            const OIS::MouseState &ms = _mouse->getMouseState();
            return vec3(ms.X.rel, ms.Y.rel, ms.Z.rel) * mouseSens;
        }

        bool input::mod(modifier m) const
        {
            switch(m)
            {
                case M_SHIFT:
                    return _keys->isModifierDown(OIS::Keyboard::Shift);
                break;

                case M_CTRL:
                    return _keys->isModifierDown(OIS::Keyboard::Ctrl);
                break;

                case M_ALT:
                    return _keys->isModifierDown(OIS::Keyboard::Alt);
                break;
            }
            return false;
        }

        strToKeyMap _strToKeyMap;
        keyToStrMap _keyToStrMap;

        void _fillConversionMap()
        {
            using namespace OIS;
            _keyToStrMap[KC_UNASSIGNED] = "KC_UNASSIGNED";
            _strToKeyMap["KC_UNASSIGNED"] = KC_UNASSIGNED;
            _keyToStrMap[KC_ESCAPE] = "KC_ESCAPE";
            _strToKeyMap["KC_ESCAPE"] = KC_ESCAPE;
            _keyToStrMap[KC_1] = "KC_1";
            _strToKeyMap["KC_1"] = KC_1;
            _keyToStrMap[KC_2] = "KC_2";
            _strToKeyMap["KC_2"] = KC_2;
            _keyToStrMap[KC_3] = "KC_3";
            _strToKeyMap["KC_3"] = KC_3;
            _keyToStrMap[KC_4] = "KC_4";
            _strToKeyMap["KC_4"] = KC_4;
            _keyToStrMap[KC_5] = "KC_5";
            _strToKeyMap["KC_5"] = KC_5;
            _keyToStrMap[KC_6] = "KC_6";
            _strToKeyMap["KC_6"] = KC_6;
            _keyToStrMap[KC_7] = "KC_7";
            _strToKeyMap["KC_7"] = KC_7;
            _keyToStrMap[KC_8] = "KC_8";
            _strToKeyMap["KC_8"] = KC_8;
            _keyToStrMap[KC_9] = "KC_9";
            _strToKeyMap["KC_9"] = KC_9;
            _keyToStrMap[KC_0] = "KC_0";
            _strToKeyMap["KC_0"] = KC_0;
            _keyToStrMap[KC_MINUS] = "KC_MINUS";
            _strToKeyMap["KC_MINUS"] = KC_MINUS;
            _keyToStrMap[KC_EQUALS] = "KC_EQUALS";
            _strToKeyMap["KC_EQUALS"] = KC_EQUALS;
            _keyToStrMap[KC_BACK] = "KC_BACK";
            _strToKeyMap["KC_BACK"] = KC_BACK;
            _keyToStrMap[KC_TAB] = "KC_TAB";
            _strToKeyMap["KC_TAB"] = KC_TAB;
            _keyToStrMap[KC_Q] = "KC_Q";
            _strToKeyMap["KC_Q"] = KC_Q;
            _keyToStrMap[KC_W] = "KC_W";
            _strToKeyMap["KC_W"] = KC_W;
            _keyToStrMap[KC_E] = "KC_E";
            _strToKeyMap["KC_E"] = KC_E;
            _keyToStrMap[KC_R] = "KC_R";
            _strToKeyMap["KC_R"] = KC_R;
            _keyToStrMap[KC_T] = "KC_T";
            _strToKeyMap["KC_T"] = KC_T;
            _keyToStrMap[KC_Y] = "KC_Y";
            _strToKeyMap["KC_Y"] = KC_Y;
            _keyToStrMap[KC_U] = "KC_U";
            _strToKeyMap["KC_U"] = KC_U;
            _keyToStrMap[KC_I] = "KC_I";
            _strToKeyMap["KC_I"] = KC_I;
            _keyToStrMap[KC_O] = "KC_O";
            _strToKeyMap["KC_O"] = KC_O;
            _keyToStrMap[KC_P] = "KC_P";
            _strToKeyMap["KC_P"] = KC_P;
            _keyToStrMap[KC_LBRACKET] = "KC_LBRACKET";
            _strToKeyMap["KC_LBRACKET"] = KC_LBRACKET;
            _keyToStrMap[KC_RBRACKET] = "KC_RBRACKET";
            _strToKeyMap["KC_RBRACKET"] = KC_RBRACKET;
            _keyToStrMap[KC_RETURN] = "KC_RETURN";
            _strToKeyMap["KC_RETURN"] = KC_RETURN;
            _keyToStrMap[KC_LCONTROL] = "KC_LCONTROL";
            _strToKeyMap["KC_LCONTROL"] = KC_LCONTROL;
            _keyToStrMap[KC_A] = "KC_A";
            _strToKeyMap["KC_A"] = KC_A;
            _keyToStrMap[KC_S] = "KC_S";
            _strToKeyMap["KC_S"] = KC_S;
            _keyToStrMap[KC_D] = "KC_D";
            _strToKeyMap["KC_D"] = KC_D;
            _keyToStrMap[KC_F] = "KC_F";
            _strToKeyMap["KC_F"] = KC_F;
            _keyToStrMap[KC_G] = "KC_G";
            _strToKeyMap["KC_G"] = KC_G;
            _keyToStrMap[KC_H] = "KC_H";
            _strToKeyMap["KC_H"] = KC_H;
            _keyToStrMap[KC_J] = "KC_J";
            _strToKeyMap["KC_J"] = KC_J;
            _keyToStrMap[KC_K] = "KC_K";
            _strToKeyMap["KC_K"] = KC_K;
            _keyToStrMap[KC_L] = "KC_L";
            _strToKeyMap["KC_L"] = KC_L;
            _keyToStrMap[KC_SEMICOLON] = "KC_SEMICOLON";
            _strToKeyMap["KC_SEMICOLON"] = KC_SEMICOLON;
            _keyToStrMap[KC_APOSTROPHE] = "KC_APOSTROPHE";
            _strToKeyMap["KC_APOSTROPHE"] = KC_APOSTROPHE;
            _keyToStrMap[KC_GRAVE] = "KC_GRAVE";
            _strToKeyMap["KC_GRAVE"] = KC_GRAVE;
            _keyToStrMap[KC_LSHIFT] = "KC_LSHIFT";
            _strToKeyMap["KC_LSHIFT"] = KC_LSHIFT;
            _keyToStrMap[KC_BACKSLASH] = "KC_BACKSLASH";
            _strToKeyMap["KC_BACKSLASH"] = KC_BACKSLASH;
            _keyToStrMap[KC_Z] = "KC_Z";
            _strToKeyMap["KC_Z"] = KC_Z;
            _keyToStrMap[KC_X] = "KC_X";
            _strToKeyMap["KC_X"] = KC_X;
            _keyToStrMap[KC_C] = "KC_C";
            _strToKeyMap["KC_C"] = KC_C;
            _keyToStrMap[KC_V] = "KC_V";
            _strToKeyMap["KC_V"] = KC_V;
            _keyToStrMap[KC_B] = "KC_B";
            _strToKeyMap["KC_B"] = KC_B;
            _keyToStrMap[KC_N] = "KC_N";
            _strToKeyMap["KC_N"] = KC_N;
            _keyToStrMap[KC_M] = "KC_M";
            _strToKeyMap["KC_M"] = KC_M;
            _keyToStrMap[KC_COMMA] = "KC_COMMA";
            _strToKeyMap["KC_COMMA"] = KC_COMMA;
            _keyToStrMap[KC_PERIOD] = "KC_PERIOD";
            _strToKeyMap["KC_PERIOD"] = KC_PERIOD;
            _keyToStrMap[KC_SLASH] = "KC_SLASH";
            _strToKeyMap["KC_SLASH"] = KC_SLASH;
            _keyToStrMap[KC_RSHIFT] = "KC_RSHIFT";
            _strToKeyMap["KC_RSHIFT"] = KC_RSHIFT;
            _keyToStrMap[KC_MULTIPLY] = "KC_MULTIPLY";
            _strToKeyMap["KC_MULTIPLY"] = KC_MULTIPLY;
            _keyToStrMap[KC_LMENU] = "KC_LMENU";
            _strToKeyMap["KC_LMENU"] = KC_LMENU;
            _keyToStrMap[KC_SPACE] = "KC_SPACE";
            _strToKeyMap["KC_SPACE"] = KC_SPACE;
            _keyToStrMap[KC_CAPITAL] = "KC_CAPITAL";
            _strToKeyMap["KC_CAPITAL"] = KC_CAPITAL;
            _keyToStrMap[KC_F1] = "KC_F1";
            _strToKeyMap["KC_F1"] = KC_F1;
            _keyToStrMap[KC_F2] = "KC_F2";
            _strToKeyMap["KC_F2"] = KC_F2;
            _keyToStrMap[KC_F3] = "KC_F3";
            _strToKeyMap["KC_F3"] = KC_F3;
            _keyToStrMap[KC_F4] = "KC_F4";
            _strToKeyMap["KC_F4"] = KC_F4;
            _keyToStrMap[KC_F5] = "KC_F5";
            _strToKeyMap["KC_F5"] = KC_F5;
            _keyToStrMap[KC_F6] = "KC_F6";
            _strToKeyMap["KC_F6"] = KC_F6;
            _keyToStrMap[KC_F7] = "KC_F7";
            _strToKeyMap["KC_F7"] = KC_F7;
            _keyToStrMap[KC_F8] = "KC_F8";
            _strToKeyMap["KC_F8"] = KC_F8;
            _keyToStrMap[KC_F9] = "KC_F9";
            _strToKeyMap["KC_F9"] = KC_F9;
            _keyToStrMap[KC_F10] = "KC_F10";
            _strToKeyMap["KC_F10"] = KC_F10;
            _keyToStrMap[KC_NUMLOCK] = "KC_NUMLOCK";
            _strToKeyMap["KC_NUMLOCK"] = KC_NUMLOCK;
            _keyToStrMap[KC_SCROLL] = "KC_SCROLL";
            _strToKeyMap["KC_SCROLL"] = KC_SCROLL;
            _keyToStrMap[KC_NUMPAD7] = "KC_NUMPAD7";
            _strToKeyMap["KC_NUMPAD7"] = KC_NUMPAD7;
            _keyToStrMap[KC_NUMPAD8] = "KC_NUMPAD8";
            _strToKeyMap["KC_NUMPAD8"] = KC_NUMPAD8;
            _keyToStrMap[KC_NUMPAD9] = "KC_NUMPAD9";
            _strToKeyMap["KC_NUMPAD9"] = KC_NUMPAD9;
            _keyToStrMap[KC_SUBTRACT] = "KC_SUBTRACT";
            _strToKeyMap["KC_SUBTRACT"] = KC_SUBTRACT;
            _keyToStrMap[KC_NUMPAD4] = "KC_NUMPAD4";
            _strToKeyMap["KC_NUMPAD4"] = KC_NUMPAD4;
            _keyToStrMap[KC_NUMPAD5] = "KC_NUMPAD5";
            _strToKeyMap["KC_NUMPAD5"] = KC_NUMPAD5;
            _keyToStrMap[KC_NUMPAD6] = "KC_NUMPAD6";
            _strToKeyMap["KC_NUMPAD6"] = KC_NUMPAD6;
            _keyToStrMap[KC_ADD] = "KC_ADD";
            _strToKeyMap["KC_ADD"] = KC_ADD;
            _keyToStrMap[KC_NUMPAD1] = "KC_NUMPAD1";
            _strToKeyMap["KC_NUMPAD1"] = KC_NUMPAD1;
            _keyToStrMap[KC_NUMPAD2] = "KC_NUMPAD2";
            _strToKeyMap["KC_NUMPAD2"] = KC_NUMPAD2;
            _keyToStrMap[KC_NUMPAD3] = "KC_NUMPAD3";
            _strToKeyMap["KC_NUMPAD3"] = KC_NUMPAD3;
            _keyToStrMap[KC_NUMPAD0] = "KC_NUMPAD0";
            _strToKeyMap["KC_NUMPAD0"] = KC_NUMPAD0;
            _keyToStrMap[KC_DECIMAL] = "KC_DECIMAL";
            _strToKeyMap["KC_DECIMAL"] = KC_DECIMAL;
            _keyToStrMap[KC_OEM_102] = "KC_OEM_102";
            _strToKeyMap["KC_OEM_102"] = KC_OEM_102;
            _keyToStrMap[KC_F11] = "KC_F11";
            _strToKeyMap["KC_F11"] = KC_F11;
            _keyToStrMap[KC_F12] = "KC_F12";
            _strToKeyMap["KC_F12"] = KC_F12;
            _keyToStrMap[KC_F13] = "KC_F13";
            _strToKeyMap["KC_F13"] = KC_F13;
            _keyToStrMap[KC_F14] = "KC_F14";
            _strToKeyMap["KC_F14"] = KC_F14;
            _keyToStrMap[KC_F15] = "KC_F15";
            _strToKeyMap["KC_F15"] = KC_F15;
            _keyToStrMap[KC_KANA] = "KC_KANA";
            _strToKeyMap["KC_KANA"] = KC_KANA;
            _keyToStrMap[KC_ABNT_C1] = "KC_ABNT_C1";
            _strToKeyMap["KC_ABNT_C1"] = KC_ABNT_C1;
            _keyToStrMap[KC_CONVERT] = "KC_CONVERT";
            _strToKeyMap["KC_CONVERT"] = KC_CONVERT;
            _keyToStrMap[KC_NOCONVERT] = "KC_NOCONVERT";
            _strToKeyMap["KC_NOCONVERT"] = KC_NOCONVERT;
            _keyToStrMap[KC_YEN] = "KC_YEN";
            _strToKeyMap["KC_YEN"] = KC_YEN;
            _keyToStrMap[KC_ABNT_C2] = "KC_ABNT_C2";
            _strToKeyMap["KC_ABNT_C2"] = KC_ABNT_C2;
            _keyToStrMap[KC_NUMPADEQUALS] = "KC_NUMPADEQUALS";
            _strToKeyMap["KC_NUMPADEQUALS"] = KC_NUMPADEQUALS;
            _keyToStrMap[KC_PREVTRACK] = "KC_PREVTRACK";
            _strToKeyMap["KC_PREVTRACK"] = KC_PREVTRACK;
//            _keyToStrMap[KC_CIRCUMFLEX] = "KC_CIRCUMFLEX";
//            _strToKeyMap["KC_CIRCUMFLEX"] = KC_CIRCUMFLEX;
            _keyToStrMap[KC_AT] = "KC_AT";
            _strToKeyMap["KC_AT"] = KC_AT;
            _keyToStrMap[KC_COLON] = "KC_COLON";
            _strToKeyMap["KC_COLON"] = KC_COLON;
            _keyToStrMap[KC_UNDERLINE] = "KC_UNDERLINE";
            _strToKeyMap["KC_UNDERLINE"] = KC_UNDERLINE;
            _keyToStrMap[KC_KANJI] = "KC_KANJI";
            _strToKeyMap["KC_KANJI"] = KC_KANJI;
            _keyToStrMap[KC_STOP] = "KC_STOP";
            _strToKeyMap["KC_STOP"] = KC_STOP;
            _keyToStrMap[KC_AX] = "KC_AX";
            _strToKeyMap["KC_AX"] = KC_AX;
            _keyToStrMap[KC_UNLABELED] = "KC_UNLABELED";
            _strToKeyMap["KC_UNLABELED"] = KC_UNLABELED;
            _keyToStrMap[KC_NEXTTRACK] = "KC_NEXTTRACK";
            _strToKeyMap["KC_NEXTTRACK"] = KC_NEXTTRACK;
            _keyToStrMap[KC_NUMPADENTER] = "KC_NUMPADENTER";
            _strToKeyMap["KC_NUMPADENTER"] = KC_NUMPADENTER;
            _keyToStrMap[KC_RCONTROL] = "KC_RCONTROL";
            _strToKeyMap["KC_RCONTROL"] = KC_RCONTROL;
            _keyToStrMap[KC_MUTE] = "KC_MUTE";
            _strToKeyMap["KC_MUTE"] = KC_MUTE;
            _keyToStrMap[KC_CALCULATOR] = "KC_CALCULATOR";
            _strToKeyMap["KC_CALCULATOR"] = KC_CALCULATOR;
            _keyToStrMap[KC_PLAYPAUSE] = "KC_PLAYPAUSE";
            _strToKeyMap["KC_PLAYPAUSE"] = KC_PLAYPAUSE;
            _keyToStrMap[KC_MEDIASTOP] = "KC_MEDIASTOP";
            _strToKeyMap["KC_MEDIASTOP"] = KC_MEDIASTOP;
            _keyToStrMap[KC_VOLUMEDOWN] = "KC_VOLUMEDOWN";
            _strToKeyMap["KC_VOLUMEDOWN"] = KC_VOLUMEDOWN;
            _keyToStrMap[KC_VOLUMEUP] = "KC_VOLUMEUP";
            _strToKeyMap["KC_VOLUMEUP"] = KC_VOLUMEUP;
            _keyToStrMap[KC_WEBHOME] = "KC_WEBHOME";
            _strToKeyMap["KC_WEBHOME"] = KC_WEBHOME;
            _keyToStrMap[KC_NUMPADCOMMA] = "KC_NUMPADCOMMA";
            _strToKeyMap["KC_NUMPADCOMMA"] = KC_NUMPADCOMMA;
            _keyToStrMap[KC_DIVIDE] = "KC_DIVIDE";
            _strToKeyMap["KC_DIVIDE"] = KC_DIVIDE;
            _keyToStrMap[KC_SYSRQ] = "KC_SYSRQ";
            _strToKeyMap["KC_SYSRQ"] = KC_SYSRQ;
            _keyToStrMap[KC_RMENU] = "KC_RMENU";
            _strToKeyMap["KC_RMENU"] = KC_RMENU;
            _keyToStrMap[KC_PAUSE] = "KC_PAUSE";
            _strToKeyMap["KC_PAUSE"] = KC_PAUSE;
            _keyToStrMap[KC_HOME] = "KC_HOME";
            _strToKeyMap["KC_HOME"] = KC_HOME;
            _keyToStrMap[KC_UP] = "KC_UP";
            _strToKeyMap["KC_UP"] = KC_UP;
            _keyToStrMap[KC_PGUP] = "KC_PGUP";
            _strToKeyMap["KC_PGUP"] = KC_PGUP;
            _keyToStrMap[KC_LEFT] = "KC_LEFT";
            _strToKeyMap["KC_LEFT"] = KC_LEFT;
            _keyToStrMap[KC_RIGHT] = "KC_RIGHT";
            _strToKeyMap["KC_RIGHT"] = KC_RIGHT;
            _keyToStrMap[KC_END] = "KC_END";
            _strToKeyMap["KC_END"] = KC_END;
            _keyToStrMap[KC_DOWN] = "KC_DOWN";
            _strToKeyMap["KC_DOWN"] = KC_DOWN;
            _keyToStrMap[KC_PGDOWN] = "KC_PGDOWN";
            _strToKeyMap["KC_PGDOWN"] = KC_PGDOWN;
            _keyToStrMap[KC_INSERT] = "KC_INSERT";
            _strToKeyMap["KC_INSERT"] = KC_INSERT;
            _keyToStrMap[KC_DELETE] = "KC_DELETE";
            _strToKeyMap["KC_DELETE"] = KC_DELETE;
            _keyToStrMap[KC_LWIN] = "KC_LWIN";
            _strToKeyMap["KC_LWIN"] = KC_LWIN;
            _keyToStrMap[KC_RWIN] = "KC_RWIN";
            _strToKeyMap["KC_RWIN"] = KC_RWIN;
            _keyToStrMap[KC_APPS] = "KC_APPS";
            _strToKeyMap["KC_APPS"] = KC_APPS;
            _keyToStrMap[KC_POWER] = "KC_POWER";
            _strToKeyMap["KC_POWER"] = KC_POWER;
            _keyToStrMap[KC_SLEEP] = "KC_SLEEP";
            _strToKeyMap["KC_SLEEP"] = KC_SLEEP;
            _keyToStrMap[KC_WAKE] = "KC_WAKE";
            _strToKeyMap["KC_WAKE"] = KC_WAKE;
            _keyToStrMap[KC_WEBSEARCH] = "KC_WEBSEARCH";
            _strToKeyMap["KC_WEBSEARCH"] = KC_WEBSEARCH;
            _keyToStrMap[KC_WEBFAVORITES] = "KC_WEBFAVORITES";
            _strToKeyMap["KC_WEBFAVORITES"] = KC_WEBFAVORITES;
            _keyToStrMap[KC_WEBREFRESH] = "KC_WEBREFRESH";
            _strToKeyMap["KC_WEBREFRESH"] = KC_WEBREFRESH;
            _keyToStrMap[KC_WEBSTOP] = "KC_WEBSTOP";
            _strToKeyMap["KC_WEBSTOP"] = KC_WEBSTOP;
            _keyToStrMap[KC_WEBFORWARD] = "KC_WEBFORWARD";
            _strToKeyMap["KC_WEBFORWARD"] = KC_WEBFORWARD;
            _keyToStrMap[KC_WEBBACK] = "KC_WEBBACK";
            _strToKeyMap["KC_WEBBACK"] = KC_WEBBACK;
            _keyToStrMap[KC_MYCOMPUTER] = "KC_MYCOMPUTER";
            _strToKeyMap["KC_MYCOMPUTER"] = KC_MYCOMPUTER;
            _keyToStrMap[KC_MAIL] = "KC_MAIL";
            _strToKeyMap["KC_MAIL"] = KC_MAIL;
            _keyToStrMap[KC_MEDIASELECT] = "KC_MEDIASELECT";
            _strToKeyMap["KC_MEDIASELECT"] = KC_MEDIASELECT;
        }


        input::input(size_t hwnd):
            _mgr(NULL),
            _keys(NULL),
            _mouse(NULL),
            mouseSens(1)
        {
        	// Create inputs handlers for keyboard and mouse
            _mgr = OIS::InputManager::createInputSystem(hwnd);
            _keys = (OIS::Keyboard*)_mgr->createInputObject(OIS::OISKeyboard, true);
            _mouse = (OIS::Mouse*)_mgr->createInputObject(OIS::OISMouse, true);

            // We wanna use unicode
            _keys->setTextTranslation(OIS::Keyboard::Unicode);
            if (_keys->getTextTranslation() != OIS::Keyboard::Unicode)
            {
                log("Failed to set unicode translation for keys, trying ascii");
                _keys->setTextTranslation(OIS::Keyboard::Ascii);
                if (_keys->getTextTranslation() != OIS::Keyboard::Ascii)
                    log("Failed to set ascii translation for keys");
            }

            _fillConversionMap();
        }

        input::~input()
        {
        	// Unset input handlers
            _mgr->destroyInputObject(_mouse);
            _mgr->destroyInputObject(_keys);
            OIS::InputManager::destroyInputSystem(_mgr);
        }

        bool input::isHeld(unsigned c) const
        {
            if (c <= LAST_KEY)
                return _keys->isKeyDown(OIS::KeyCode(c));
            if (c <= NUM_INPUT_VALUES)
                return _mouse->getMouseState().buttonDown(convert(mouseButton(c)));
            return false;
        }

        void input::grab(OIS::KeyListener *k, OIS::MouseListener *m) const
        {
            _keys->setEventCallback(k);
            _mouse->setEventCallback(m);
        }

        void input::capture() const
        {
            _keys->capture();
            _mouse->capture();
        }

        void input::resize(unsigned w, unsigned h) const
        {
            const OIS::MouseState &ms = _mouse->getMouseState();
            ms.width = w;
            ms.height = h;
        }
    }
}
