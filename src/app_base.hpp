 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef APP_BASE_HPP_INCLUDED
#define APP_BASE_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include "types.hpp"

namespace engine
{
    namespace gfx
    {
        class root;
    }

    namespace input
    {
        class input;
    }

    namespace snd
    {
        class system;
    }
}
namespace game
{
    class stateManager;
}

class appBase
{

    protected:

        virtual void configure(
            engine::gfx::root &root,
            engine::input::input &input,
            engine::snd::system &soundSys,
            game::stateManager &stateMgr,
            engine::programOptions *opts) = 0;

    public:

        appBase()
        {
        }

        virtual ~appBase()
        {
        }

        int run(int argc, char **argv);

};

typedef boost::shared_ptr<appBase> appBasePtr;



#endif // APP_BASE_HPP_INCLUDED
