 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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
            _text(new text(_name + "::_text", true))
        {
            update
//                ("texture", string("gui/back_med.png"))
                ("size", vec2(96, 32))
                ("draggable", false);

            child(_text)
                ("text", string("<text>"))
                ("colour", colour::White)
                ("clickable", true)
                .callback("onClick", boost::bind(&button::_clickText, this, _1));
//                ("selectParent", true);
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

        boost::any button::attrib(const string &name) const
        {
            if (name == "text") return _text->attrib("text");

            else return panel::attrib(name);

            return boost::any();
        }

        widget &button::update(const string &name, const boost::any &val)
        {
            if (name == "text") _text->update("text", val);

            else return panel::update(name, val);

            return *this;
        }

        void button::tick(real dt)
        {
            panel::tick(dt);
            _text->update
                ("position", size * vec2(0.5, 0.5) - _text->size * vec2(0.5, 0.5));
        }
    }
}
