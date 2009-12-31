 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <cmath>
#include <numeric>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>

#include "window.hpp"
#include "safe_any.hpp"

namespace engine
{


    namespace gui
    {

        const size_t TITLEBAR_HEIGHT = 16;

        titlebar::titlebar(const string &name, bool internal):
            panel(name, internal),
            _text(new text("text", true)),
            _closeButton(new panel("closeButton", true))
        {
            _interfaces.push_back(WIDGET_TITLE_BAR);
            addChild(_text);
            _text->setText("untitled");

            addChild(_closeButton);
            _closeButton->textureName("button_close.png");
            _closeButton->fitTextureSize();
            _closeButton->draggable = false;
            _closeButton->decorate = false;

            size = vec2(128, TITLEBAR_HEIGHT);
            clippable = false;
            obeyOffset = false;
        }

        void titlebar::tick(real dt)
        {
            panel::tick(dt);
            _closeButton->position = vec2(size.x - _closeButton->size.x, 0);
            _text->position = vec2(size.x / 2 - _text->size.x / 2, 0);
        }

        string titlebar::getSkinType() const
        {
            return "titlebar";
        }

        window::window(const string &name, bool internal):
            scrollablePanel(name, internal),
            _resizer(new panel("resizer", true)),
            _titlebar(new titlebar("titlebar", true))
        {
            _interfaces.push_back(WIDGET_WINDOW);

            addChild(_resizer);
            _resizer->visible = false;
            _resizer->obeyOffset = false;
            _resizer->textureName("resizer.png");
            _resizer->fitTextureSize();
            _resizer->decorate = false;
            _resizer->setCallback("onDrag", boost::bind(&window::_resize, this, _1));

            addChild(_titlebar);
            _titlebar->child("closeButton")->setCallback("onClick", boost::bind(&window::_close, this, _1));

            size = vec2(256, 256);
        }

        window::~window()
        {
        }

        void window::_close(const event &e)
        {
            visible = false;
            _fireCallback("onClose", e.misc);
        }

        void window::_resize(const event &e)
        {
            vec2 v = SAFE_ANY_CAST_T(vec2, e.misc, vec2(0, 0));
            if (size.x + v.x > 64)
                size.x += v.x;
            if (size.y + v.y > 16)
                size.y += v.y;

            _resizer->position = size - _resizer->size;
            _titlebar->size.x = size.x;
        }

        void window::tick(real dt)
        {
            scrollablePanel::tick(dt);

            _resizer->position = size - _resizer->size;

            _titlebar->size.x = size.x;
            _titlebar->position.y = -_titlebar->rect().w;
        }

        string window::getSkinType() const
        {
            return "window";
        }

        void window::resizable(bool b)
        {
            _resizer->visible = b;
        }

        void window::closable(bool b)
        {
            _titlebar->child("closeButton")->visible = b;
        }

        void window::title(const string &str)
        {
//            _titlebar->child("text")->setText(str);
        }

    }

}

