 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SLEEP_HPP_INCLUDED
#define SLEEP_HPP_INCLUDED

#ifdef _WINDOWS
#include <windows.h>
#else
#endif

namespace engine
{
    namespace time
    {

        inline void sleep(unsigned ms)
        {
            #ifdef _WINDOWS
            Sleep(ms);
            #endif
        }

    }
}

#endif // SLEEP_HPP_INCLUDED
