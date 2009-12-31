 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PANEL_HPP_INCLUDED
#define PANEL_HPP_INCLUDED

#include "pre_ogre.hpp"
#include "widget.hpp"
#include "types.hpp"

namespace engine
{

    namespace gui
    {

        class panel;
        typedef boost::shared_ptr<panel> panelPtr;

        class panel: public widget
        {
            protected:

                string _textureName;

                virtual void _renderThis(Ogre::Viewport *vp);

            public:

                panel(const string &name, bool internal = false);
                virtual ~panel();

                void textureName(const string &str);
                const string &textureName() const { return _textureName; }
                void fitTextureSize();

                virtual vec2 skinOffset(const align_t &a = A_TL) const;
                virtual string getSkinType() const;
        };

    }

}

#endif // PANEL_HPP_INCLUDED
