 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef RENDERER_HPP_INCLUDED
#define RENDERER_HPP_INCLUDED

#include <boost/shared_ptr.hpp>
//#include <boost/signal.hpp>
//#include <boost/function.hpp>

#include <OgreColourValue.h>

#include "pre_ogre.hpp"
#include "string.hpp"

#include "id.hpp"
#include "quad.hpp"

namespace engine
{

    namespace gfx
    {

        enum
        {
            MAIN_GROUP = 1 << 0,
            OVERLAY_GROUP = 1 << 1,
            NUM_GROUPS
        };

        class renderer
        {

            private:

                static idGen _idGen;

            protected:

                Ogre::SceneManager *_sceneMgr;
                Ogre::Viewport *_viewport;
                Ogre::Camera *_camera;
                // quad to render full screen
                quad _quad;

                string _name;

                real _dt;

            public:

//                typedef boost::function<void()> preRenderFunc;
//
//                boost::signal<void(const renderer&, Ogre::Camera *cam,
//                    Ogre::Viewport *vp, const preRenderFunc&)> renderSignal;

                renderer(Ogre::SceneManager *sceneMgr, Ogre::Viewport *viewport);
                virtual ~renderer();

                virtual void render() const = 0;
                virtual void component(const string &name, bool active)
                {
                    // component based usage
                }

                virtual void tick(real dt);

                // render a full screen quad
                void fsquad(Ogre::Pass *p, Ogre::Viewport *vp, bool beginEnd = true) const;

                // render the scene, using the render signals
//                void renderScene(Ogre::Camera *cam, Ogre::Viewport *vp, const preRenderFunc &prefix) const;

                Ogre::SceneManager *sceneMgr() const { return _sceneMgr; }
                Ogre::Viewport *viewport() const { return _viewport; }
                Ogre::Camera *camera() const { return _camera; }
                Ogre::RenderSystem *renderSys() const;
                Ogre::RenderTarget *rt() const;

                unsigned width() const;
                unsigned height() const;

                void clearFrameBuffer(unsigned buffers =
                    Ogre::FBT_COLOUR | Ogre::FBT_DEPTH | Ogre::FBT_STENCIL,
                    const colour &c = colour::Black,
                    float d = 1, unsigned short s = 0) const;

//                void render(Ogre::SceneNode *node) const;

                //void beginRender() const;
                //void endRender() const;
        };


        typedef boost::shared_ptr<renderer> rendererPtr;


    }

}


#endif // RENDERER_HPP_INCLUDED
