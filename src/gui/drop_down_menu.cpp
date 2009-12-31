 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <cmath>
#include <numeric>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>

#include "drop_down_menu.hpp"
#include "safe_any.hpp"
#include "layout.hpp"

namespace engine
{


    namespace gui
    {

        string dropDownMenuChoices::getSkinType() const
        {
            return "dropDownMenu";
        }

        dropDownMenu::dropDownMenu(const string &name, bool internal):
            panel(name, internal),
            _selection(new text("selection", true)),
            _choices(new dropDownMenuChoices("choices", true))
        {
            _interfaces.push_back(WIDGET_DROP_DOWN_MENU);

            addChild(_selection);
            _selection->selectParent = true;

            addChild(_choices);
            _choices->clippable = false;
            _choices->obeyOffset = false;
            _choices->draggable = false;
            _choices->selectParent = true;
            _choices->visible = false;

            _choices->setLayout(layoutPtr(new verticalLayout()));
        }

        dropDownMenu::~dropDownMenu()
        {
        }

        void dropDownMenu::tick(real dt)
        {
            panel::tick(dt);

            _choices->position = vec2(0, rect().w);
            _choices->size = vec2(size.x, 256);
            _choices->visible = active;
        }

        void dropDownMenu::_select(const event &e)
        {
//            string text = SAFE_ANY_CAST(string, e.caller.attrib("text"));
//
//            _selection->setText(text);
//
//            _fireCallback("onSelect", text);
        }

        string dropDownMenu::getSkinType() const
        {
            return "dropDownMenu";
        }

        void dropDownMenu::addChoice(const string &str)
        {
            layoutPtr lay = _choices->getLayout();
            assert(lay);

            textPtr choice(new text("choice" + boost::lexical_cast<string>(lay->numItems())));
            _choices->addChild(choice);
            lay->addWidget(choice);

            choice->setText(str);
            choice->selectParent = false;
            choice->draggable = false;
            choice->clickable = true;
            choice->setCallback("onClick", boost::bind(&dropDownMenu::_select, this, _1));
        }

        void dropDownMenu::adjustSize(const vec2 &v)
        {
            // x == pixels, y == y * text height
            size = v * vec2(1, _selection->info('X').h);
        }

        void dropDownMenu::mouseUp(const vec2 &p, mouseButton b)
        {
            panel::mouseUp(p, b);

            active = !active;
        }

    }

}

