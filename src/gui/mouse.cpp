 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

/*
 * Update list:
 * date
 * comments
 */

#include <OgrePass.h>
#include <OgreViewport.h>

#include "mouse.hpp"

namespace engine
{
    namespace gui
    {
        mouse::mouse(const string &name):
            panel(name)
        {
            (*this)
                ("texture", string("gui/mouse.png"))
                ("autoSize");
        }

        mouse::~mouse()
        {
        }

        // TODO: Remove rendering cursor by hands

//        void mouse::_renderThis(Ogre::Viewport *vp)
//        {
//            // render directly, no caching here
//            _pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
//            _pass->getTextureUnitState(0)->setTextureName(_textureName);
//            _quad->render(_pass, vp);
//        }

    }
}
