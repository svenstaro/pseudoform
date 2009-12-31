 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef LOG_HPP_INCLUDED
#define LOG_HPP_INCLUDED

#include "string.hpp"
//#include <OgreLogManager.h>

namespace engine
{

    extern const string LOG_NAME;

    void log(const string &str);

}

#endif // LOG_HPP_INCLUDED
