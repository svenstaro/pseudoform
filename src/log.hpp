 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef LOG_HPP_INCLUDED
#define LOG_HPP_INCLUDED

#include "string.hpp"

/**
 * @namespace engine
 * @brief Consist of functions related to the game-engine
 */
namespace engine
{
	/// Global engine log file name
    extern const string LOG_NAME;

    /**
     * @brief Write information into engine log file
     * @param str information to put
     */
    void log(const string &str);
}

#endif // LOG_HPP_INCLUDED
