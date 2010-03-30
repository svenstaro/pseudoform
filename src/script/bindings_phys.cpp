 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <chaiscript/chaiscript.hpp>

#include "bindings.hpp"

#include "phys/world.hpp"
#include "phys/body.hpp"

#include "log.hpp"
#include "types.hpp"

namespace engine
{

    namespace script
    {
        void bindPhys(script &scr)
        {
            using namespace chaiscript;
            using namespace phys;

//            module(L, "phys")
//            [
//                class_<gravity_t>("gravity_t")
//                    .def(constructor<>())
//                    .def_readwrite("v", &gravity_t::v)
//                    .def_readwrite("strength", &gravity_t::strength)
//                    .def_readwrite("isPoint", &gravity_t::isPoint)
//                    .def_readwrite("enabled", &gravity_t::enabled)
//
//                ,class_<world/*, worldPtr*/>("world")
//                    //.def(constructor<>())
//                    // bind setting
//                    .def("freq", (void (world::*)(real))&world::freq)
//                    // bind getting
//                    .def("freq", (real (world::*)() const)&world::freq)
//                    .def("tick", &world::tick)
//                    .def("threads", &world::threads)
//                    .def("solver", &world::solver)
//                    .def("friction", &world::friction)
//                    .def("platform", &world::platform)
//                    .def_readwrite("gravity", &world::gravity)
//                    .enum_("constants")
//                    [
//                        value("PLAT_DEFAULT", (unsigned)world::PLAT_DEFAULT),
//                        value("PLAT_OPTMIZED", (unsigned)world::PLAT_OPTMIZED),
//                        value("SOLVER_EXACT", (unsigned)world::SOLVER_EXACT),
//                        value("SOLVER_ADAPTIVE", (unsigned)world::SOLVER_ADAPTIVE),
//                        value("FRIC_EXACT", (unsigned)world::FRIC_EXACT),
//                        value("FRIC_ADAPTIVE", (unsigned)world::FRIC_ADAPTIVE)
//                    ]
//            ];
        }

    }

}
