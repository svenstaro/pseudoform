 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include "pre_ogre.hpp"

#include <OgreTexture.h>

#include "types.hpp"

/**
 * @namespace engine
 * @brief Consist of functions related to the game-engine
 */
namespace engine
{
	/**
	 * @namespace gfx
	 * @brief Consist of 3d render tools
	 */
    namespace gfx
    {
		/**
		 * @brief Get render pass for given technique
		 * @param matName material name to get pass for
		 * @param t technique to search pass in
		 * @param p pass name to get
		 * @param clone if true it makes another copy of given material
		 *
		 * @return pointer to the ogre pass object
		 */
        Ogre::Pass *getPass(const string &matName, size_t t, size_t p, bool clone = false);

		/**
		 * @brief Get render pass for given technique
		 * @param matName material name to get pass for
		 * @param t technique to search pass in
		 * @param passName string pass name to get
		 * @param clone if true it makes another copy of given material
		 *
		 * @return pointer to the  ogre pass object
		 */
        Ogre::Pass *getPass(const string &matName, size_t t, const string &passName, bool clone = false);

		/**
		 * @brief Load texture with given name and handle it
		 * @param name texture to get
		 *
		 * @return pointer to the loaded texture
		 */
        Ogre::TexturePtr getTexture(const string &name);

        /**
         * @brief Check texture for existing in loaded resources
         * @param name texture name to check
         */
        bool hasTexture(const string &name);

        /**
         * @brief Check file existing for the given texture name
         * @param name texture file name to check
         */
        bool hasTextureFile(const string &name);
    }
}

#endif // UTILS_HPP_INCLUDED
