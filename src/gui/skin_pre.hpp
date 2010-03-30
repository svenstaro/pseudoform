 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SKIN_PRE_HPP_INCLUDED
#define SKIN_PRE_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include "types.hpp"

namespace engine
{
    namespace gui
    {
		/// Widget position
        typedef string align_t;

        /**
         * \brief Widgets positions
         *
         * A_TL - TopLeft; A_T - Top; A_TR - TopRight
         * A_R - Right; A_BR - BottomRight; A_B - Bottom
         * A_BL - BottomLeft; A_L - Left; A_MI - Middle
         */
        extern const align_t A_TL, A_T, A_TR, A_R, A_BR, A_B, A_BL, A_L, A_MI;

        class skin;

        /// Shared pointer for the skin object
        typedef boost::shared_ptr<skin> skinPtr;
    }
}

#endif // SKIN_PRE_HPP_INCLUDED
