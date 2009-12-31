 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef TEXT_BOX_HPP_INCLUDED
#define TEXT_BOX_HPP_INCLUDED

#include "panel.hpp"
#include "text.hpp"

namespace engine
{
    namespace gui
    {

        class textBox: public panel
        {
            protected:

                textPtr _text;
                panelPtr _caret;

                string _str;
                size_t _index;

                void _updateText();
                void _updateCaret();
                void _updateTextAndCaret();

            public:

                textBox(const string &name, bool internal = false);
                virtual ~textBox() {}

                void tick(real dt);

                void keyDown(specialKey k);
                void keyDown(unsigned k);
                void mouseDown(const vec2 &p, mouseButton b);

                const string &getText() const { return _str; }
                void setText(const string &str);
                void clear();
                void adjustSize(const vec2 &v);

                string getSkinType() const;
        };

        typedef boost::shared_ptr<textBox> textBoxPtr;
    }
}

#endif // TEXT_BOX_HPP_INCLUDED
