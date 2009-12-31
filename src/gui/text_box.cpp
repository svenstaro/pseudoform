 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <cmath>

#include "text_box.hpp"
#include "safe_any.hpp"

namespace engine
{
    namespace gui
    {

        textBox::textBox(const string &name, bool internal):
            panel(name, internal),
            _text(new text("text", true)),
            _caret(new panel("caret", true)),
            _index(0)
        {
            _interfaces.push_back(WIDGET_TEXT_BOX);

            addChild(_text);
            _text->selectParent = true;

            addChild(_caret);
            _caret->draggable = false;
            _caret->size = vec2(2, _text->info('X').h);
            _caret->textureName("gui/back_dark.png");
            _caret->decorate = false;
            _caret->visible = false;

            draggable = false;
            size = vec2(192, _text->info('X').h);
        }

        string textBox::getSkinType() const
        {
            return "textBox";
        }

        void textBox::keyDown(specialKey k)
        {
            panel::keyDown(k);
            switch (k)
            {
                case SK_ENTER:
                    _fireCallback("onEnter", _str);
                break;

                case SK_BACKSPACE:
                    if (!_str.empty() && _index > 0)
                    {
                        _str.erase(--_index, 1);
                        _updateTextAndCaret();
                    }
                break;

                case SK_LEFT:
                    if (_index > 0)
                    {
                        --_index;
                        _updateTextAndCaret();
                    }
                break;

                case SK_RIGHT:
                    if (_index < _str.size())
                    {
                        ++_index;
                        _updateTextAndCaret();
                    }
                break;

                default:
                break;
            }
        }

        void textBox::mouseDown(const vec2 &p, mouseButton b)
        {
            panel::mouseDown(p, b);
            assert(contains(p));

            const text::glyphInfo &base = _text->info(unsigned('X'));

            vec2 offset = p - _caret->derivedPosition();
            int i = int(_index) + int(offset.x / base.w + 0.5);
            if (i < 0) i = 0;

            _index = size_t(i);
            if (_index > _str.size()) _index = _str.size();

            _updateCaret();
        }

        void textBox::keyDown(unsigned k)
        {
            char c = char(k);
            if (c == ' ' || c == '\t' || _text->info(k).valid)
            {
                // if we're at the end, then add the character
                if (_index >= _str.size())
                {
                    _str += c;
                    ++_index;
                }
                // otherwise, insert it at the caret index
                else _str.insert(_index++, 1, c);

                _updateTextAndCaret();
            }
        }

        void textBox::adjustSize(const vec2 &v)
        {
            // x == pixels, y == y * text height
            size = v * vec2(1, _text->info('X').h);
        }

        void textBox::clear()
        {
            _str.clear();
            _index = 0;
            _updateTextAndCaret();
        }

        void textBox::setText(const string &str)
        {
            _str = str;
            _index = 0;
            _updateTextAndCaret();
        }

        void textBox::tick(real dt)
        {
            panel::tick(dt);

            _caret->visible = active;
        }

        void textBox::_updateText()
        {
            _text->setText(_str);

            // use textSize() because the update is deferred until _renderThis()
            vec2 s = _text->textSize(_str.substr(0, _index));
            // if the text is too big, move it to the left/up
            _text->position = vec2(std::min(size.x - s.x, 0.0f), std::min(size.y - s.y, 0.0f));
        }

        void textBox::_updateCaret()
        {
            vec2 p = _text->textEnd(_str.substr(0, _index));
            _caret->position = p;
        }

        void textBox::_updateTextAndCaret()
        {
            _updateText();
            _updateCaret();
        }

    }
}
