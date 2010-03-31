 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "gui_disp.hpp"
#include "types.hpp"
#include "mouse_button.hpp"

namespace engine
{
    namespace gui
    {
        bool guiDisp::keyPressed(const OIS::KeyEvent &e)
        {
            if (!_dispatchSheet)
                return true;

            switch (e.key)
            {
                case OIS::KC_RETURN:
                    _dispatchSheet->keyDown(SK_ENTER);
                break;

                case OIS::KC_BACK:
                    _dispatchSheet->keyDown(SK_BACKSPACE);
                break;

                case OIS::KC_LEFT:
                    _dispatchSheet->keyDown(SK_LEFT);
                break;

                case OIS::KC_RIGHT:
                    _dispatchSheet->keyDown(SK_RIGHT);
                break;

                case OIS::KC_UP:
                    _dispatchSheet->keyDown(SK_UP);
                break;

                case OIS::KC_DOWN:
                    _dispatchSheet->keyDown(SK_DOWN);
                break;

                default:
                    _dispatchSheet->keyDown(e.text);
                break;
            }

            return true;
        }

        bool guiDisp::keyReleased(const OIS::KeyEvent &e)
        {
            if (!_dispatchSheet)
                return true;

            return true;
        }

        bool guiDisp::mouseMoved(const OIS::MouseEvent &e)
        {
//            if (!_gui.mouseVisible())
//                return true;
            if (!_dispatchSheet)
                return true;

            _dispatchSheet->mouseMove(vec2(e.state.X.rel, e.state.Y.rel));
            if (int(e.state.Z.rel))
                _dispatchSheet->mouseScroll(e.state.Z.rel * -0.25);

            //_gui._mgr->injectMousePosition(e.state.X.abs, e.state.Y.abs);
            //_gui._mgr->injectMouseMove(e.state.X.rel, e.state.Y.rel);

            return true;
        }

        bool guiDisp::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
        {
//            if (!_gui.mouseVisible())
//                return true;
            if (!_dispatchSheet)
                return true;

            _dispatchSheet->mouseDown(mouseButton(id));
            //_gui._mgr->injectMouseButtonDown(QuickGUI::MouseButtonID(id));

            return true;
        }

        bool guiDisp::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
        {
//            if (!_gui.mouseVisible())
//                return true;
            if (!_dispatchSheet)
                return true;

            _dispatchSheet->mouseUp(mouseButton(id));

            //_gui._mgr->injectMouseButtonUp(QuickGUI::MouseButtonID(id));

            return true;
        }
    }
}
