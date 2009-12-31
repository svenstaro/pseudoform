 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef DROP_DOWN_MENU_HPP_INCLUDED
#define DROP_DOWN_MENU_HPP_INCLUDED

#include "scrollable_panel.hpp"
#include "text.hpp"

namespace engine
{


    namespace gui
    {

        class dropDownMenuChoices: public scrollablePanel
        {
            public:

                dropDownMenuChoices(const string &name, bool internal = false):
                    scrollablePanel(name, internal)
                {
                }
                ~dropDownMenuChoices()
                {
                }

                string getSkinType() const;

        };

        class dropDownMenu: public panel
        {
            protected:

                textPtr _selection;

                scrollablePanelPtr _choices;

//                unsigned _numChoices;

                void _select(const event &e);

            public:

                dropDownMenu(const string &name, bool internal = false);
                ~dropDownMenu();

                void addChoice(const string &str);
                void adjustSize(const vec2 &v);

                void tick(real dt);

                void mouseUp(const vec2 &p, mouseButton b);

                string getSkinType() const;

        };

        typedef boost::shared_ptr<dropDownMenu> dropDownMenuPtr;

    }


}

#endif // DROP_DOWN_MENU_HPP_INCLUDED
