 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef BUTTON_HPP_INCLUDED
#define BUTTON_HPP_INCLUDED

#include "panel.hpp"
#include "text.hpp"

namespace engine
{
    namespace gui
    {
		/**
		 * Button widget
		 */
        class button;

        /// Shared pointer for the button
        typedef boost::shared_ptr<button> buttonPtr;

        class button: public panel
        {
            protected:
				/// Button label text
                textPtr _text;

				/**
				 * Click event
				 * @param e called event additional data
				 */
                void _clickText(const event &e);

            public:
				/**
				 * Constructor
				 */
                button(const string &name, bool internal = false);

                /**
                 * Destructor
                 */
                ~button();

                /**
                 * Update button from global application cycle
                 */
                void tick(real dt);

                /**
                 * Get current object type
                 */
                string getSkinType() const;

                widget &update(const string &name, const boost::any &val);
                boost::any attrib(const string &name) const;
        };
    }
}

#endif // BUTTON_HPP_INCLUDED
