 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef DIR_HPP_INCLUDED
#define DIR_HPP_INCLUDED

#include "string.hpp"

/**
 * @namespace engine
 * @brief Consist of functions related to the game-engine
 */
namespace engine
{
	/**
	 * @brief Create dir in filesystem
	 * @param dir name if need directory
	 */
    void makeDir(const string &dir);

    /**
     * @brief Remove file from filesystem
     * @param file file to remove
     */
    void removeFile(const string &file);

    /**
     * @brief Copy file
     * @param source file to copy
     * @param dest destination of copy
     */
    void copyFile(const string &source, const string &dest);
}

#endif // DIR_HPP_INCLUDED
