 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef DIR_HPP_INCLUDED
#define DIR_HPP_INCLUDED

#include "string.hpp"

namespace engine
{

    void makeDir(const string &dir);
    void removeFile(const string &file);
    void copyFile(const string &source, const string &dest);
}

#endif // DIR_HPP_INCLUDED
