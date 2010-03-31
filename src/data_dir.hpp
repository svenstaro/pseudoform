 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef DATA_DIR_HPP_INCLUDED
#define DATA_DIR_HPP_INCLUDED

#include "types.hpp"

/**
 * @namespace engine
 * @brief Consist of functions related to the game-engine
 */
namespace engine
{
	/// Path to the directory with game content
    extern string DATA_DIR;

    /// Path to the root dir of game engine
    extern string ROOT_DIR;

    /**
     * @brief Get data and root dirs from config file
     */
    void configureDirs();
}

#endif // DATA_DIR_HPP_INCLUDED
