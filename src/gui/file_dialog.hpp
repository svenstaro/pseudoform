 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef FILE_DIALOG_HPP_INCLUDED
#define FILE_DIALOG_HPP_INCLUDED

#include <list>

#include <OgreLog.h>

#include "window.hpp"
#include "text_box.hpp"
#include "text.hpp"
#include "button.hpp"

namespace engine
{
    namespace gui
    {

        class fileDialog: public window
        {
            protected:

                struct dir
                {
                    panelPtr icon;
                    textPtr text;

                    dir(const panelPtr &icon, const textPtr &t):
                        icon(icon), text(t) {}
                };

                typedef std::list<dir> dirList;
                dirList _dirs;
                widgetPtr _selection;

                string _currentDir;

                textBoxPtr _input;
                buttonPtr _go;

                void _clearDirs();
                void _exploreDir(const string &str);

                void _enterInput(const event &e);
                void _clickGo(const event &e);
                void _clickDir(const event &e);
                void _doubleClickDir(const event &e);
                void _confirm(const string &str);
                void _fireConfirmCallback(const string &str);

            public:

                bool relativeToDataDir;

                fileDialog(const string &name);
                ~fileDialog();

                void setDir(const string &str);

                void tick(real dt);
        };

    }
}


#endif // FILE_DIALOG_HPP_INCLUDED
