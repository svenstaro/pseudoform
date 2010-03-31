 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <stdexcept>

#include <OIS/OISKeyboard.h>

#include "gfx/root.hpp"
#include "input/input.hpp"
#include "states/state.hpp"
#include "snd/system.hpp"

#include "data_dir.hpp"
#include "app_base.hpp"
#include "fs_ops.hpp"

int appBase::run(int argc, char **argv)
{
    using namespace engine;

    engine::programOptions opts;
    for (int i = 1; i < argc; ++i) // Skip the first one (executable name)
        opts.push_back(engine::string(argv[i]));

    // Load DATA_DIR and ROOT_DIR
    configureDirs();

    // We need to make sure that we have some settings to load
    // TODO: Do something with default bindings. Maybe will need in chaiscript something like this
    //copyFile(DATA_DIR + "scripts/bind_default.lua", DATA_DIR + "scripts/bind.lua");
    copyFile(DATA_DIR + "scripts/root_default.xml", DATA_DIR + "scripts/root.xml");

    // Create render application object
    gfx::rootPtr root;
    try
    {
        root.reset(new gfx::root(DATA_DIR + "scripts/root.xml", &opts));
    }
    catch(const std::exception &e)
    {
        std::cerr << "Root error: " << e.what();
        return 0xf33df00d;
    }

    // Create and manage input system
    input::input input(root->hwnd());
    input.mouseSens = 0.25;
    input.resize(root->width(), root->height());

    snd::system soundSys;

    game::stateManager stateMgr;

    configure(*root, input, soundSys, stateMgr, &opts);

    // Game loop
    while(root->windowIsOpen())
    {
        // Prepare window
        root->begin();
        // Capture input
        input();

        // root->begin() and input() can result in a closed window
        // Don't render if the window closed!
        if (!root->windowIsOpen())
            break;

        soundSys.tick();

        // Tick states
        if (!stateMgr.tick())
            break;

        // Render states
        stateMgr.render();

        // Finish window rendering
        root->end();

        // We don't need to time this since the screenshot will take at least 250 ms
        if (input.keys()->isKeyDown(OIS::KC_SYSRQ))
            root->screenShot();
    }

    return 0;
}
