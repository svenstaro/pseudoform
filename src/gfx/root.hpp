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

namespace engine
{

    namespace gfx
    {

        class root: public Ogre::WindowEventListener
        {

            private:
                Ogre::Root *_root;
                Ogre::RenderWindow *_window;
                Ogre::RenderSystem *_renderSys;
                Ogre::SceneManager *_sceneMgr;
                Ogre::Viewport *_viewport;
                Ogre::Camera *_camera;

                Ogre::ManualObject *_quad;

                void destroyQuad();
                void buildQuad(Ogre::Camera *cam);

                bool _windowIsOpen;

            public:

                Ogre::RenderWindow *window() const { return _window; }
                Ogre::RenderSystem *renderSys() const { return _renderSys; }
                Ogre::SceneManager *sceneMgr() const { return _sceneMgr; }
                Ogre::Viewport *viewport() const { return _viewport; }
                Ogre::Camera *camera() const { return _camera; }

                bool windowIsOpen() const { return _windowIsOpen; }

                root(const string &cfg, programOptions *opts = NULL);
                ~root();

                void begin() const;
                void end() const;

                size_t hwnd() const;

                unsigned width() const;
                unsigned height() const;

                void screenShot();

                void windowClosed(Ogre::RenderWindow*) { _windowIsOpen = false; }

                const string svn_rev;
        };


        typedef boost::shared_ptr<root> rootPtr;


    }

}

#endif // ROOT_HPP_INCLUDED
