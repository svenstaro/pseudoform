 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

#include "character.hpp"

namespace game
{
	/**
	 * @class player
	 * @brief Player character for game world
	 * @see class character
	 */
    class player: public character
    {
        private:
			/// Character's camera node
            Ogre::SceneNode *_cameraNode;
//            Ogre::Bone *_headBone, *_cameraBone;

            /// Camera position from character's body
            engine::vec3 _camPos;
            /// Camera rotation
            engine::quat _camRot;

//            Ogre::AnimationState *_standAnim;

        public:
            /**
             * @brief Get position of character's camera
             * @return vector3, camera position
             */
            engine::vec3 getCamPos() const { return _camPos; }
            /**
             * @brief Get rotating of character's camera
             * @return quaternion represents camera rotations
             */
            engine::quat getCamRot() const { return _camRot; }

//            virtual void pitch(float f);

            static const interfaceType TYPE;

            /**
             * Constructor
             */
            player(const engine::string &name, const genericProp::constructParams &params);
            /**
             * Destructor
             */
            virtual ~player();

            virtual void tick(engine::real dt);

//            virtual void pos(const engine::vec3 &v);
//            virtual void matrix(const engine::mat4 &m);
    };

    typedef boost::shared_ptr<player> playerPtr;

    /**
     * @class playerFactory
     * @see class entityFactory, class entity
     */
    class playerFactory: public entityFactory
    {
        private:
            genericProp::constructParams _params;

        public:
			/**
			 * Constructor
			 */
            playerFactory(const genericProp::constructParams &params):
                entityFactory(player::TYPE),
                _params(params)
            {
            }

            entityPtr create(const engine::string &name) const;
    };

    /**
     * Shared pointer to the current class
     */
    typedef boost::shared_ptr<playerFactory> playerFactoryPtr;
}

#endif // PLAYER_HPP_INCLUDED
