 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <OgreLogManager.h>

#include "console.hpp"
#include "safe_any.hpp"

namespace engine
{
    namespace gui
    {

        console::console(const string &name, bool internal):
            window(name, internal),
            _curLine(_lines.end()),
            _input(new textBox("input", true)),
            _listeningToLog(false)
        {
            _interfaces.push_back(WIDGET_CONSOLE);

            addChild(_input);
            _input->adjustSize(vec2(size.x, 1));
            _input->setCallback("onEnter", boost::bind(&console::_enterInput, this, _1));
            _input->setCallback("onSpecialKey", boost::bind(&console::_inputUpDown, this, _1));
            _input->obeyOffset = false;
            _input->draggable = false;
            _input->clippable = false;
        }

        console::~console()
        {
            if (_listeningToLog)
                listenToLog(false); // just in case
        }

        void console::messageLogged(const string &str, Ogre::LogMessageLevel lvl, bool maskDebug, const string &name)
        {
            switch (lvl)
            {
                case Ogre::LML_TRIVIAL:
                    addLine(str);
                break;

                case Ogre::LML_NORMAL:
                    addLine(str, colour(0, 0, 0));
                break;

                case Ogre::LML_CRITICAL:
                    addLine(str, colour(1, 0, 0));
                break;
            }
        }

        void console::listenToLog(bool b)
        {
            _listeningToLog = b;
            if (!b)
                Ogre::LogManager::getSingleton().getLog(LOG_NAME)->removeListener(this);
            else
                Ogre::LogManager::getSingleton().getLog(LOG_NAME)->addListener(this);
        }

        void console::addLine(const string &str, const colour &c)
        {
            textPtr t(new text("line" + boost::lexical_cast<string>(_lines.size())));
            addChild(t);

            float y = 0;
            if (!_lines.empty())
            // use actualSize() since the size variable might not be updated yet
            // if the text is not yet rendered
                y = _lines.back()->position.y + _lines.back()->actualSize().y;

            t->setText(str);
            // if no lines, then at 0 Y, but otherwise, at the previous line Y + prev. line height
            t->position = vec2(0, y);
            t->draggable = false;
            t->selectParent = true;

            _lines.push_back(t);

            // reset our current line
            _curLine = _lines.end(); --_curLine;
        }

        void console::clear()
        {
            BOOST_FOREACH(textPtr &i, _lines)
            {
                removeChild(i->name());
            }
            _lines.clear();
        }

        void console::_enterInput(const event &e)
        {
            string str = SAFE_ANY_CAST(string, e.misc);
            addLine(str, colour::White);
            // raise an event of our own
            _fireCallback("onEnter", str);
            // clear input
            _input->clear();
        }

        void console::_inputUpDown(const event &e)
        {
            if (_curLine == _lines.end())
                return;

            specialKey k = SAFE_ANY_CAST(specialKey, e.misc);

            if (k == SK_UP && _curLine != _lines.begin())
            {
                --_curLine;
                _input->setText((*_curLine)->getText());
            }
            else if (k == SK_DOWN)
            {
                if (++_curLine != _lines.end())
                    _input->setText((*_curLine)->getText());
                else
                    _input->clear();
            }
        }

        void console::tick(real dt)
        {
            window::tick(dt);

            float height = rect().w;
            if (_xScrollbar->visible)
                height += _xScrollbar->rect().w;

            _input->adjustSize(vec2(size.x, 1));
            _input->position = vec2(0, height);
        }

    }

}
