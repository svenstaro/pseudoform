 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <chaiscript/chaiscript.hpp>

#include "bindings.hpp"

#include "snd/system.hpp"

#include "log.hpp"
#include "types.hpp"

namespace engine
{

    namespace script
    {
        void bindSnd(script &scr)
        {
            using namespace chaiscript;
            using namespace snd;

//            module(L, "phys")
//            [
//                class_<sound, soundPtr>("sound")
//                    .def("radii", &sound::radii)
//                    .def("pos", &sound::pos)
//                    .def("volume", (float (sound::*)() const)&sound::volume)
//                    .def("volume", (void (sound::*)(float))&sound::volume)
//                    .def("playing", &sound::playing)
//                    .def("pause", &sound::pause)
//                    .def("paused", &sound::paused)
//                    .def("resume", &sound::resume)
//                    .def("loop", &sound::loop)
//                    .def("stop", &sound::stop)
//
//                ,class_<system>("system")
//                    .def("play", &system::play)
//
//            ];
        }

    }

}
