 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "check_box.hpp"
#include "safe_any.hpp"

namespace engine
{
    namespace gui
    {

        checkBox::checkBox(const string &name, bool internal):
            panel(name, internal),
            _check(new panel("check", true))
        {
            _interfaces.push_back(WIDGET_CHECK_BOX);

            size = vec2(16, 16);
            draggable = false;
            decorate = true;

            addChild(_check);
            _check->textureName("checkbox_tix.png");
            _check->fitTextureSize();
            _check->visible = false;
            _check->selectParent = true;
            _check->decorate = false;
            _check->draggable = false;
        }

        checkBox::~checkBox()
        {
        }

        string checkBox::getSkinType() const
        {
            return "checkBox";
        }

        bool checkBox::checked() const
        {
            return _check->visible;
        }

        void checkBox::checked(bool b)
        {
            _check->visible = b;
        }

        void checkBox::mouseUp(const vec2 &p, mouseButton b)
        {
            if (b == MB_LEFT)
            {
                _check->visible = !_check->visible;
                _fireCallback("onCheck", _check->visible);
            }
        }

    }

}
