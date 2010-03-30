 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef APP_BASE_HPP_INCLUDED
#define APP_BASE_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

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
		 * @class root
		 * @brief Main render class, manages all Ogre stuff
		 */
		class root;
	}

	/**
	 * @namespace input
	 * @brief Consist of tools for crossing input devices
	 */
	namespace input
	{
		/**
		 * @class input
		 * @brief Manages input system
		 */
		class input;
	}

	/**
	 * @namespace snd
	 * @brief Consist of tools for playing sounds in game world
	 */
	namespace snd
	{
		/**
		 * @class system
		 * @brief Manages sound system
		 */
		class system;
	}
}

/**
 * @namespace game
 * @brief Consist of tools for building game world
 */
namespace game
{
	/**
	 * @class stateManager
	 * @brief Manages switching between game states
	 * @see class state
	 */
	class stateManager;
}

/**
 * @class appBase
 * @brief Main application
 */
class appBase
{
	protected:
		/**
		 * @brief Configure application object
		 *
		 * @param root render class
		 * @param input input class
		 * @param soundSys sound system handler
		 * @param stateMgr state manager handler
		 * @param opts program argument options
		 */
		virtual void configure(
			engine::gfx::root &root,
			engine::input::input &input,
			engine::snd::system &soundSys,
			game::stateManager &stateMgr,
			engine::programOptions *opts) = 0;

	public:
		/**
		 * Constructor
		 */
		appBase() { }

		/**
		 * Destructor
		 */
		virtual ~appBase() { }

		/**
		 * @brief Begin to receive window events
		 * @param argc number of given arguments
		 * @param argv pointer to the arguments array
		 */
		int run(int argc, char **argv);
};

/**
 * Shared pointer to the current class
 */
typedef boost::shared_ptr<appBase> appBasePtr;

#endif // APP_BASE_HPP_INCLUDED
