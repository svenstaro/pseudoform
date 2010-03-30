 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SCRIPT_HPP_INCLUDED
#define SCRIPT_HPP_INCLUDED

#include "string.hpp"

namespace chaiscript
{
    template<typename T>
    class ChaiScript_System;
    class Dispatch_Engine;

    typedef ChaiScript_System<Dispatch_Engine> ChaiScript;
}

namespace engine
{
    namespace script
    {

        class script
        {
            private:

                chaiscript::ChaiScript *_chai;

                engine::string _error;

            public:

                script();
                ~script();

                bool file(const engine::string &f);
                bool str(const engine::string &str);
                void clear();

                chaiscript::ChaiScript *chai() { return _chai; }

                engine::string error();
        };

    }

}

#endif // SCRIPT_HPP_INCLUDED
