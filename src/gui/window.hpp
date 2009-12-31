 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include "scrollable_panel.hpp"
#include "scrollbar.hpp"
#include "text.hpp"

namespace engine
{


    namespace gui
    {

        class titlebar: public panel
        {
            protected:

                textPtr _text;

                panelPtr _closeButton;

            public:

                titlebar(const string &name, bool internal = false);
                ~titlebar() {}

                string getSkinType() const;

                void tick(real dt);

                // don't select titlebars, select their windows
                widget *select() { return _parent ? _parent->select() : NULL; }
        };

        typedef boost::shared_ptr<titlebar> titlebarPtr;

        class window: public scrollablePanel
        {
            protected:

                panelPtr _resizer;
                titlebarPtr _titlebar;

                void _close(const event &e);
                void _resize(const event &e);

            public:

                window(const string &name, bool internal = false);
                virtual ~window();

                virtual void tick(real dt);

                void title(const string &str);
                void resizable(bool b);
                void closable(bool b);

                virtual string getSkinType() const;

        };

        typedef boost::shared_ptr<window> windowPtr;

    }


}

#endif // WINDOW_HPP_INCLUDED
