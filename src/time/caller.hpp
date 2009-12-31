 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef CALLER_HPP_INCLUDED
#define CALLER_HPP_INCLUDED

#include <boost/function.hpp>

#include "types.hpp"

namespace engine
{


    namespace time
    {

        class caller
        {
            private:

                real _time;

            public:

                boost::function<void()> call;
                real interval;

                caller(): _time(0), interval(1) {}
                caller(const boost::function<void()> &call, real interval):
                    _time(0),
                    call(call),
                    interval(interval)
                {
                }

                void operator()(real dt)
                {
                    _time += dt;
                    if (_time >= interval)
                    {
                        call();
                        _time = 0;
                    }
                }
        };

    }


}

#endif // CALLER_HPP_INCLUDED
