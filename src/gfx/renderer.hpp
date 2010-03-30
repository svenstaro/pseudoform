 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef RENDERER_HPP_INCLUDED
#define RENDERER_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include <OgreColourValue.h>

#include "pre_ogre.hpp"
#include "string.hpp"

#include "id.hpp"
#include "quad.hpp"

namespace engine
{
    namespace gfx
    {
		/// Render group enum
        enum
        {
            MAIN_GROUP = 1 << 0,
            OVERLAY_GROUP = 1 << 1,
            NUM_GROUPS
        };

        /**
         * @class renderer
         */
        class renderer
        {
            private:
				/// List of object names
                static idGen _idGen;

            protected:
				/// Chosen scene manager
                Ogre::SceneManager *_sceneMgr;

                /// Viewport
                Ogre::Viewport *_viewport;

                /// Main camera
                Ogre::Camera *_camera;

                /// Used for rendering fullscreen quad
                quad _quad;

                /// Name of current renderer
                string _name;

                /// Time between frames
                real _dt;

            public:
                /**
                 * Constructor
                 * @param sceneMgr scene manager to use
                 * @param viewport viewport to draw in
                 */
                renderer(Ogre::SceneManager *sceneMgr, Ogre::Viewport *viewport);

                /**
                 * Destructor
                 */
                virtual ~renderer();

                /**
                 * @brief Draw components
                 */
                virtual void render() const = 0;

                /**
                 * TODO wtf
                 */
                virtual void component(const string &name, bool active)
                {
                    // component based usage
                }

                /**
                 * @brief Used with game timer
                 */
                virtual void tick(real dt);

                /**
                 * @brief Render fullscreen quad
                 * @param p ogre pass to use for rendering
                 * @param vp viewport to render quad in
                 * @param beginEnd TODO
                 */
                void fsquad(Ogre::Pass *p, Ogre::Viewport *vp, bool beginEnd = true) const;

                /// Get scene manager
                Ogre::SceneManager *sceneMgr() const { return _sceneMgr; }
                /// Get viewport
                Ogre::Viewport *viewport() const { return _viewport; }
                /// Get camera
                Ogre::Camera *camera() const { return _camera; }
                /// Get render system
                Ogre::RenderSystem *renderSys() const;
                /// Get render target
                Ogre::RenderTarget *rt() const;

                /**
                 * @brief Get width of renderer
                 */
                unsigned width() const;

                /**
                 * @brief Get height of renderer
                 */
                unsigned height() const;

                /**
                 * @brief Clear frame buffer
                 * @param buffers additional settings for clearing
                 *
                 * @remarks here is default values for buffers param:
                 * FBT_COLOUR - default color to fill in framebuffer
                 * FBT_DEPTH - clear depth buffer
                 * FBT_STENCIL - clear stencil buffer too
                 */
                void clearFrameBuffer(unsigned buffers =
                    Ogre::FBT_COLOUR | Ogre::FBT_DEPTH | Ogre::FBT_STENCIL,
                    const colour &c = colour::Black,
                    float d = 1, unsigned short s = 0) const;

                // TODO: WTF?
                //void render(Ogre::SceneNode *node) const;

                //void beginRender() const;
                //void endRender() const;
        };

        /**
         * Shared pointer to the current class
         */
        typedef boost::shared_ptr<renderer> rendererPtr;
    }
}

#endif // RENDERER_HPP_INCLUDED
