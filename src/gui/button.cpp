 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "button.hpp"
#include "text.hpp"

namespace engine
{
    namespace gui
    {

        button::button(const string &name, bool internal):
            panel(name, internal),
            _text(new text("text", true))
        {
            _interfaces.push_back(WIDGET_BUTTON);

            size = vec2(96, 32);
            draggable = false;

            addChild(_text);
            _text->clickable = true;
            _text->setCallback("onClick", boost::bind(&button::_clickText, this, _1));
        }

        button::~button()
        {
        }

        string button::getSkinType() const
        {
            return "button";
        }

        void button::_clickText(const event &e)
        {
            _fireCallback("onClick", e.misc);
        }

        void button::setText(const string &str)
        {
            _text->setText(str);
        }

        void button::tick(real dt)
        {
            panel::tick(dt);
            _text->position = size * vec2(0.5, 0.5) - _text->size * vec2(0.5, 0.5);
        }

    }

}
