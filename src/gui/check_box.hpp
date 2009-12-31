 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef CHECK_BOX_HPP_INCLUDED
#define CHECK_BOX_HPP_INCLUDED

#include "panel.hpp"
#include "text.hpp"

namespace engine
{

    namespace gui
    {

        class checkBox;
        typedef boost::shared_ptr<checkBox> checkBoxPtr;

        class checkBox: public panel
        {
            protected:

                panelPtr _check;

            public:

                checkBox(const string &name, bool internal = false);
                ~checkBox();

                string getSkinType() const;

                void checked(bool b);
                bool checked() const;

                void mouseUp(const vec2 &p, mouseButton b);
        };

    }

}

#endif // CHECK_BOX_HPP_INCLUDED
