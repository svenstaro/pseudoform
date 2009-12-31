 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

//#include <QuickGUIRoot.h>

#include <OgreResourceGroupManager.h>

//#include "gfx/utils.hpp"

#include "logo.hpp"
#include "log.hpp"

namespace game
{

    void logoState::suspend()
    {
        assert(false && "logoState state should not be suspended!");
    }

    void logoState::resume()
    {
        assert(false && "logoState state should not be resumed!");
    }

    logoState::logoState(Ogre::Viewport *viewport, engine::real dur):
        _viewport(viewport),
        _duration(dur) // default to 3 second duration
    {
        // hide mouse
//        _gui.mouseVisible(false);

//        _sheet = _gui.createSheet();
//
//        using namespace engine;
//
//        gui::rect size(0, 0, _viewport->getActualWidth(), _viewport->getActualHeight());
//        _sheet->setDimensions(size);
//
//        {
//            QuickGUI::ImageDesc d;
//            d.imageName = "startup.png";
//            _img = _sheet->createImage(d);
//        }
//
//        // center it
//        gfx::dim dims = gfx::dims("startup.png");
//        gui::rect imgSize(
//            size.w * 0.5 - dims.w * 0.5,
//            size.h * 0.5 - dims.h * 0.5, dims.w, dims.h);
//        _img->setDimensions(imgSize);

        // reset timer so we don't have any flukes
        // when loading other states
        _timer.reset();
    }

    logoState::~logoState()
    {
        //gui->destroyWidget(_img);
//        _img = NULL;
//        _gui.destroySheet(_sheet);
//        _sheet = NULL;
    }

    void logoState::render()
    {
//        _gui.activeSheet(_sheet);
//        _gui.render(_viewport);
    }

    void logoState::tick(stateManager &mgr)
    {
        _timer();

        if (_timer.total() > _duration)
        {
            // exceeded duration, pop ourselves
            mgr.pop();
        }
        // otherwise, fade in/out
        else
        {
//            using namespace engine;
//            real delta = _timer.total() / _duration * 2.0 - 1; // [0..1] -> [-1..1]
//            real alpha = delta * delta; // quadratic
//
//            gfx::alpha(_img->getMaterialName(), alpha);
//
//            _gui.tick(_timer.delta());
        }
    }

}


