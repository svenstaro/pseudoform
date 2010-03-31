 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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
		// TODO: Comment panel class
        class panel;

        /// Base type for shared pointer onto panel object
        typedef boost::shared_ptr<panel> panelPtr;

        class panel: public widget
        {
            protected:
				/// Name of texture to use with this panel
                string _textureName;

                /// Render this current object manually
                virtual void _renderThis(Ogre::Viewport *vp);

            public:
				/**
				 * Constructor
				 */
                panel(const string &name, bool internal = false);

                /**
                 * Destructor
                 */
                virtual ~panel();

                // TODO: comment this
                virtual vec2 skinOffset(const align_t &a = A_TL) const;

                /**
                 * Get the type of current object
                 */
                virtual string getSkinType() const;

                /**
                 * Update given attribute name of current object
                 * @param name attribute name to work with
                 */
                virtual widget &update(const string &name, const boost::any &val);

                /**
                 * Get additional attribute for given object name
                 * @name attribute to get
                 */
                virtual boost::any attrib(const string &name) const;
        };
    }
}

#endif // PANEL_HPP_INCLUDED
