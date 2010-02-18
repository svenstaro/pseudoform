 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <stdexcept>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#ifdef _WINDOWS
//#include <fmod.h>
#elif _LINUX
//#include <fmodex/fmod.h>
//#include <fmodex/fmodlinux.h>
#endif

#include "system.hpp"
#include "types.hpp"
#include "log.hpp"
#include "data_dir.hpp"

namespace engine
{
    namespace snd
    {

        system::system():
//            _system(NULL),
            _speed(1)
        {
//            FMOD_RESULT res = FMOD_System_Create(&_system);
//            if (res != FMOD_OK)
//                throw std::runtime_error("failed to create FMOD sound system: " + boost::lexical_cast<string>(res));
            //FMOD_LINUX_EXTRADRIVERDATA extra = { 0 }; // Not used right now
            #ifdef _WINDOWS
//            FMOD_System_SetOutput(_system, FMOD_OUTPUTTYPE_DSOUND);
            #elif _LINUX
//            FMOD_System_SetOutput(_system, FMOD_OUTPUTTYPE_ALSA);
            #endif
//            res = FMOD_System_Init(_system, 32, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, NULL);
//            if (res != FMOD_OK)
//                throw std::runtime_error("failed to init FMOD sound system: " + boost::lexical_cast<string>(res));
        }

        system::~system()
        {
//            FMOD_System_Release(_system);
        }

        void system::tick()
        {
//            FMOD_System_Update(_system);

            std::list<soundList::iterator> toRemove;

            for (soundList::iterator i = _playing.begin(); i != _playing.end(); ++i)
            {
                sound &s = **i; // iterator to smart pointer
                if (!s.paused() && !s.playing())
                    toRemove.push_back(i);
            }

            BOOST_FOREACH(soundList::iterator &i, toRemove)
                _playing.erase(i);
        }

//        FMOD_SOUND *system::_getSound(const engine::string &fn, soundMap &which)
//        {
//            soundMap::iterator i = which.find(fn);

//            if (i == which.end())
//            {
//                FMOD_SOUND *snd = NULL;

//                unsigned flags = FMOD_SOFTWARE;
//                if (&which == &_avail3D)
//                    flags |= FMOD_3D | FMOD_3D_LINEARROLLOFF;
//                else
//                    flags |= FMOD_2D;

//                FMOD_RESULT result = FMOD_System_CreateSound(
//                    _system, (DATA_DIR + fn).c_str(), flags, NULL, &snd);

//                if (result != FMOD_OK)
//                {
//                    log("could not load sound \"" + fn + "\"");
//                    return NULL;
//                }

//                which[fn] = snd;

//                return snd;
//            }

//            return i->second;
//        }

        void system::listener(const vec3 &p, const vec3 &f, const vec3 &u, int id)
        {
//            FMOD_VECTOR fp = { p.x, p.y, p.z };
//            FMOD_VECTOR ff = { f.x, f.y, f.z };
//            FMOD_VECTOR fu = { u.x, u.y, u.z };

//            FMOD_System_Set3DListenerAttributes(_system, id, &fp, NULL, &ff, &fu);
        }

        soundPtr system::play(const engine::string &fn, bool threeD)
        {
//            FMOD_SOUND *snd = _getSound(fn, threeD ? _avail3D : _avail2D);

//            if (!snd)
//                return soundPtr();

//            FMOD_CHANNEL *chan = NULL;

//            FMOD_System_PlaySound(_system, FMOD_CHANNEL_FREE, snd, false, &chan);

//            soundPtr ret(new sound(chan));

//            _playing.push_back(ret);

//            ret->speed(_speed);

//            return ret;
        }

        void system::speed(float f)
        {
            _speed = f;
            BOOST_FOREACH(soundPtr &i, _playing)
                i->speed(_speed);
        }

    }
}
