 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef CONSOLE_HPP_INCLUDED
#define CONSOLE_HPP_INCLUDED

#include <list>

#include <OgreLog.h>

#include "window.hpp"
#include "text_box.hpp"
#include "text.hpp"

namespace engine
{
    namespace gui
    {

        class console: public window, public Ogre::LogListener
        {
            protected:

                typedef std::list<textPtr> lineList;
                lineList _lines;
                lineList::iterator _curLine;

//                scrollablePanelPtr _linesPanel;
                textBoxPtr _input;

                bool _listeningToLog;

                void _enterInput(const event &e);
                void _inputUpDown(const event &e);

            public:

                void listenToLog(bool b);
                // as an Ogre::LogListener
                void messageLogged(const string &str, Ogre::LogMessageLevel lvl, bool maskDebug, const string &name);

                void addLine(const string &str, const colour &c = colour::Black);
                void clear();

                console(const string &name, bool internal = false);
                ~console();

                void tick(real dt);
        };

    }
}


#endif // CONSOLE_HPP_INCLUDED
