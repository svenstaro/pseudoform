 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifdef _WINDOWS
#include <fmod.h>
#elif _LINUX
#include <fmodex/fmod.h>
#endif

#include "sound.hpp"

#include <iostream>
namespace engine
{
    namespace snd
    {

        sound::sound(FMOD_CHANNEL *chan):
            _chan(chan),
            _origFreq(1)
        {
            FMOD_Channel_GetFrequency(_chan, &_origFreq);
        }

//        sound::sound(const sound &rhs):
//            _chan(rhs._chan),
//            _origFreq(rhs._origFreq)
//        {
//        }
//
//        sound &sound::operator =(const sound &rhs)
//        {
//            if (this == &rhs)
//                return *this;
//
//            _chan = rhs._chan;
//            _origFreq = rhs._origFreq;
//
//            return *this;
//        }

        sound::~sound()
        {
        }

        float sound::speed() const
        {
            float current = 1;
            FMOD_Channel_GetFrequency(_chan, &current);
            return current / _origFreq;
        }

        void sound::speed(float f)
        {
            FMOD_Channel_SetFrequency(_chan, _origFreq * f);
        }

        float sound::volume() const
        {
            float current = 1;
            FMOD_Channel_GetVolume(_chan, &current);
            return current;
        }

        void sound::volume(float f)
        {
            FMOD_Channel_SetVolume(_chan, f);
        }

        void sound::pos(const vec3 &v)
        {
            FMOD_VECTOR fv = { v.x, v.y, v.z };
            FMOD_Channel_Set3DAttributes(_chan, &fv, NULL);
        }

        void sound::radii(float min, float max)
        {
            FMOD_Channel_Set3DMinMaxDistance(_chan, min, max);
        }

        bool sound::playing() const
        {
            FMOD_BOOL ret;
            FMOD_Channel_IsPlaying(_chan, &ret);
            return ret;
        }

        void sound::stop()
        {
            FMOD_Channel_Stop(_chan);
        }

        void sound::loop(int num)
        {
            FMOD_Channel_SetLoopCount(_chan, num);
        }

        void sound::pause()
        {
            FMOD_Channel_SetPaused(_chan, true);
        }

        void sound::resume()
        {
            FMOD_Channel_SetPaused(_chan, false);
        }

        bool sound::paused()
        {
            FMOD_BOOL ret;
            FMOD_Channel_GetPaused(_chan, &ret);
            return ret;
        }

    }
}

