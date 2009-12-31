 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SOUND_HPP_INCLUDED
#define SOUND_HPP_INCLUDED

#ifdef _WINDOWS
#include <fmod.h>
#elif _LINUX
#include <fmodex/fmod.h>
#endif

#include <boost/shared_ptr.hpp>

#include "types.hpp"

namespace engine
{
    namespace snd
    {

        class sound
        {
            private:

                FMOD_CHANNEL *_chan;

                float _origFreq;

                sound(const sound &rhs);
                sound &operator =(const sound &rhs);

            public:

                explicit sound(FMOD_CHANNEL *chan);
                ~sound();

                float speed() const;
                void speed(float f);

                float volume() const;
                void volume(float f);

                void pos(const vec3 &v);
                void radii(float min, float max);

                bool playing() const;

                void stop();

                void loop(int num);

                void pause();
                bool paused();
                void resume();
        };

        typedef boost::shared_ptr<sound> soundPtr;

    }
}

#endif // SOUND_HPP_INCLUDED
