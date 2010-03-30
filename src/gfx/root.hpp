 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef ROOT_HPP_INCLUDED
#define ROOT_HPP_INCLUDED

#include <OgreWindowEventUtilities.h>

#include <boost/shared_ptr.hpp>

#include "pre_ogre.hpp"
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
        class root: public Ogre::WindowEventListener
        {
            private:
				/// Base root Ogre object
                Ogre::Root *_root;

                /// Handle for the window to render in
                Ogre::RenderWindow *_window;

                /// Handle to the render system
                Ogre::RenderSystem *_renderSys;

                /// Handle for the chosen scene manager
                Ogre::SceneManager *_sceneMgr;

                /// Handle to the render viewport
                Ogre::Viewport *_viewport;

                /// Handle to the main game camera
                Ogre::Camera *_camera;

                /// Quad object, used for post processing
                Ogre::ManualObject *_quad;

                /// Represents current window state
                bool _windowIsOpen;

                /**
                 * @brief Delete quad object
                 *
                 * Delete quad object used for post processing
                 */
                void destroyQuad();

                /**
                 * @brief Build quad
                 *
                 * Build quad for the given camera. Used for postprocessing
                 * @param cam camera to build quad for
                 */
                void buildQuad(Ogre::Camera *cam);

            public:

                /**
                 * @brief Get pointer to the window to render in
                 */
                Ogre::RenderWindow *window() const { return _window; }

                /**
                 * @brief Get pointer to the render system
                 */
                Ogre::RenderSystem *renderSys() const { return _renderSys; }

                /**
                 * @brief Get pointer for the chosen scene manager
                 */
                Ogre::SceneManager *sceneMgr() const { return _sceneMgr; }

                /**
                 * @brief Get the pointer for current viewport
                 */
                Ogre::Viewport *viewport() const { return _viewport; }

                /**
                 * @brief Get the pointer for the global game camera
                 */
                Ogre::Camera *camera() const { return _camera; }

                /**
                 * @brief Represents current window state
                 */
                bool windowIsOpen() const { return _windowIsOpen; }

                /**
                 * Constructor
                 */
                root(const string &cfg, programOptions *opts = NULL);

                /**
                 * Destructor
                 */
                ~root();

                /**
                 * @brief Start rendering and receiving messages
                 */
                void begin() const;

                /**
                 * @brief Stop rendering and receiving messages
                 */
                void end() const;

                /**
                 * @brief Returns window handler
                 */
                size_t hwnd() const;

                /**
                 * @brief Returns window width
                 */
                unsigned width() const;

                /**
                 * @brief Returns window height
                 */
                unsigned height() const;

                /**
                 * @brief Make screenshot
                 */
                void screenShot();

                /**
                 * @brief Close window
                 */
                void windowClosed(Ogre::RenderWindow*) { _windowIsOpen = false; }

                /// SVN revision version
                const string svn_rev;
        };

        /**
         * Shared pointer to the current class
         */
        typedef boost::shared_ptr<root> rootPtr;
    }
}

#endif // ROOT_HPP_INCLUDED
