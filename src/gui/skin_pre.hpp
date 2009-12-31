 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

        typedef string align_t;
        extern const align_t A_TL, A_T, A_TR, A_R, A_BR, A_B, A_BL, A_L, A_MI;

        class skin;

        typedef boost::shared_ptr<skin> skinPtr;
    }
}

#endif // SKIN_PRE_HPP_INCLUDED
