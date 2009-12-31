 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <stdexcept>
#include <iostream>
#include <fstream>

#include "app_base.hpp"
#include "states/editor.hpp"
#include "states/play.hpp"
#include "states/main_menu.hpp"
#include "gfx/root.hpp"

class mainApp: public appBase
{
    protected:

        void configure(
            engine::gfx::root &root,
            engine::input::input &input,
            engine::snd::system &soundSys,
            game::stateManager &stateMgr,
            engine::programOptions *opts);

};

void mainApp::configure(
    engine::gfx::root &root,
    engine::input::input &input,
    engine::snd::system &soundSys,
    game::stateManager &stateMgr,
    engine::programOptions *opts)
{
    using namespace game;

    #ifdef DEBUG_NEWTON_BUILD
    stateMgr.push(statePtr(new playState(input, soundSys, root.sceneMgr(), root.viewport(), opts)));
    #else
//    stateMgr.push(statePtr(new editorState(input, root.sceneMgr(), root.viewport(), opts)));
    stateMgr.push(statePtr(new mainMenuState(input, soundSys, root.sceneMgr(), root.viewport(), opts)));
    #endif
}

int main(int argc, char **argv)
{
    int ret = 0;
    try
    {
        appBasePtr app(new mainApp());
        ret = app->run(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::ofstream out("fatal.txt");
        out << "fatal exception: " << e.what() << "\n";
    }
    return ret;
}
