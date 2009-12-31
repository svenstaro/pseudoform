 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "file_dialog.hpp"
#include "safe_any.hpp"
#include "data_dir.hpp"

namespace fs = boost::filesystem;

namespace engine
{
    namespace gui
    {

        fileDialog::fileDialog(const string &name):
            window(name),
            _input(new textBox("input", true)),
            _go(new button("go", true)),
            relativeToDataDir(true)
        {
            _interfaces.push_back(WIDGET_FILE_DIALOG);

            addChild(_input);
            _input->adjustSize(vec2(size.x, 1));
            _input->setCallback("onEnter", boost::bind(&fileDialog::_enterInput, this, _1));
            _input->obeyOffset = false;
            _input->draggable = false;
            _input->clippable = false;

            addChild(_go);
            _go->obeyOffset = false;
            _go->clippable = false;
            _go->draggable = false;
            _go->setText("go");
            _go->setCallback("onClick", boost::bind(&fileDialog::_clickGo, this, _1));

            title("file dialog");

            _exploreDir(DATA_DIR);
        }

        fileDialog::~fileDialog()
        {
        }

        void fileDialog::_clearDirs()
        {
            BOOST_FOREACH(dir &i, _dirs)
            {
                removeChild(i.icon->name());
                removeChild(i.text->name());
            }
            _dirs.clear();
            _selection.reset();
        }

        void fileDialog::_exploreDir(const string &str)
        {
            fs::path path(str);

            if (!fs::is_directory(path))
                return;

            _currentDir = str;

            _clearDirs();

            // create parent-directory object
            {
                panelPtr icon(new panel("_dirs0_icon"));
                addChild(icon);
                icon->textureName("gui/directory.png");
                icon->position = vec2(0, 0);
                icon->fitTextureSize();
                icon->draggable = false;
                icon->decorate = false;
                icon->clickable = false;

                textPtr t(new text("_dirs0"));
                addChild(t);
                t->setText("../");
                t->position = vec2(icon->size.x + 4, 0);
                t->draggable = false;
                t->clickable = true;
                t->misc = path.branch_path().string();
                t->setCallback("onMouseDown", boost::bind(&fileDialog::_clickDir, this, _1));
                t->setCallback("onDoubleClick", boost::bind(&fileDialog::_doubleClickDir, this, _1));

                _dirs.push_back(dir(icon, t));
            }

            for (fs::directory_iterator i(path), end; i != end; ++i)
            {
                bool isDir = fs::is_directory(*i);

                string idx = boost::lexical_cast<string>(_dirs.size());

                panelPtr icon(new panel("_dirs" + idx + "_icon"));
                addChild(icon);
                icon->textureName(isDir ? string("gui/directory.png") : string("gui/file.png"));
                icon->position = vec2(0, _dirs.size() * _input->size.y);
                icon->fitTextureSize();
                icon->draggable = false;
                icon->decorate = false;
                icon->clickable = false;

                textPtr t(new text("_dirs" + idx));
                addChild(t);
                t->setText(i->path().leaf() + (isDir ? "/" : ""));
                t->position = vec2(icon->size.x + 4, _dirs.size() * _input->size.y);
                t->draggable = false;
                t->clickable = true;
                t->misc = i->path().string();
                t->setCallback("onMouseDown", boost::bind(&fileDialog::_clickDir, this, _1));
                t->setCallback("onDoubleClick", boost::bind(&fileDialog::_doubleClickDir, this, _1));

                _dirs.push_back(dir(icon, t));
            }
        }

        void fileDialog::_clickDir(const event &e)
        {
            _selection = e.caller.smartPtr();

            string str = SAFE_ANY_CAST(string, e.caller.misc);
            // we only want the leaf, not the full path
            // (which is in the misc data)
            _input->setText(fs::path(str).leaf());
        }

        void fileDialog::_doubleClickDir(const event &e)
        {
            string str = SAFE_ANY_CAST(string, e.caller.misc);

            // we don't use _confirm() here because the misc data
            // is already the full directory, not just the current
            // name relative to the current directory
            if (fs::is_directory(str))
            {
                _exploreDir(str);
                _input->clear();
            }
            else
            {
                _fireConfirmCallback(str);
            }
        }

        void fileDialog::_enterInput(const event &e)
        {
            string str = SAFE_ANY_CAST(string, e.misc);
            _confirm(str);
        }

        void fileDialog::_clickGo(const event &e)
        {
            string str = SAFE_ANY_CAST(string, _input->getText());
            _confirm(str);
        }

        void fileDialog::_confirm(const string &str)
        {
            if (str.empty())
                return;

            if (fs::is_directory(fs::path(_currentDir) / fs::path(str)))
            {
                _exploreDir((fs::path(_currentDir) / fs::path(str)).string());
                _input->clear();
            }
            else
            {
                string final = (fs::path(_currentDir) / fs::path(str)).string();
                _fireConfirmCallback(final);
            }
        }

        void fileDialog::_fireConfirmCallback(const string &str)
        {
            if (relativeToDataDir)
            {
                // gotta cut off the data directory from it
                _fireCallback("onConfirm", str.substr(DATA_DIR.size()));
            }
            else
            {
                _fireCallback("onConfirm", str);
            }
        }

        void fileDialog::tick(real dt)
        {
            window::tick(dt);

            vec4 r = rect();

            _input->adjustSize(vec2(size.x - _go->size.x, 1));
            _input->position = vec2(0, r.w);

            vec4 ir = _input->rect();

            _go->position = vec2(ir.z, r.w);
        }

    }

}
