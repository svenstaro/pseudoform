 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SOUND_HPP_INCLUDED
#define SOUND_HPP_INCLUDED

#ifdef _WINDOWS
//#include <fmod.h>
#elif _LINUX
//#include <fmodex/fmod.h>
#endif

#include <boost/shared_ptr.hpp>

#include "types.hpp"

/**
 * @namespace engine
 * @brief Consist of functions related to the game-engine
 */
namespace engine
{
	/**
	 * @namespace snd
	 * @brief Consist of tools for playing sounds in game world
	 */
    namespace snd
    {
		/**
		 * @class sound
		 * @brief Sound controller
		 */
        class sound
        {
            private:

//                FMOD_CHANNEL *_chan;

				/// Sound stream frequency
                float _origFreq;

                sound(const sound &rhs);
                sound &operator =(const sound &rhs);

            public:

//                explicit sound(FMOD_CHANNEL *chan);
                ~sound();

                /**
                 * @brief Get sound speed
                 */
                float speed() const;
                /**
                 * @brief Set sound speed
                 * @param f new speed
                 */
                void speed(float f);

                /**
                 * @brief Get sound volume
                 * @return value of current volume
                 */
                float volume() const;

                /**
                 * @brief Set new sound volume
                 * @param new volume value
                 */
                void volume(float f);

                // TODO
                void pos(const vec3 &v);
                void radii(float min, float max);

                /// Represents current sound state
                bool playing() const;

                /**
                 * @brief Stop sound playing
                 */
                void stop();

                /**
                 * @brief Play sound given number of times
                 * @param num how many times to play again
                 */
                void loop(int num);

                /**
                 * @brief Pause sound playing
                 */
                void pause();

                /// Represents sound pause state
                bool paused();

                /**
                 * @brief Resume sound playing
                 */
                void resume();
        };

        /**
         * Shared pointer to the current class
         */
        typedef boost::shared_ptr<sound> soundPtr;
    }
}

#endif // SOUND_HPP_INCLUDED
