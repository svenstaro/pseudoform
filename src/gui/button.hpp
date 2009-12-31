 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef BUTTON_HPP_INCLUDED
#define BUTTON_HPP_INCLUDED

#include "panel.hpp"
#include "text.hpp"

namespace engine
{

    namespace gui
    {

        class button;
        typedef boost::shared_ptr<button> buttonPtr;

        class button: public panel
        {
            protected:

                textPtr _text;

                void _clickText(const event &e);

            public:

                button(const string &name, bool internal = false);
                ~button();

                void tick(real dt);

                void setText(const string &str);

                string getSkinType() const;
        };

    }

}

#endif // BUTTON_HPP_INCLUDED
