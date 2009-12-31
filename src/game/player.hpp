 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

#include "character.hpp"

namespace game
{

    class player: public character
    {
        private:

            Ogre::SceneNode *_cameraNode;
//            Ogre::Bone *_headBone, *_cameraBone;

            engine::vec3 _camPos;
            engine::quat _camRot;

//            Ogre::AnimationState *_standAnim;

        public:

            engine::vec3 getCamPos() const { return _camPos; }
            engine::quat getCamRot() const { return _camRot; }

//            virtual void pitch(float f);

            static const interfaceType TYPE;

            player(const engine::string &name, const genericProp::constructParams &params);
            virtual ~player();

            virtual void tick(engine::real dt);

//            virtual void pos(const engine::vec3 &v);
//            virtual void matrix(const engine::mat4 &m);
    };

    typedef boost::shared_ptr<player> playerPtr;

    class playerFactory: public entityFactory
    {
        private:

            genericProp::constructParams _params;

        public:

            playerFactory(const genericProp::constructParams &params):
                entityFactory(player::TYPE),
                _params(params)
            {
            }

            entityPtr create(const engine::string &name) const;
    };

    typedef boost::shared_ptr<playerFactory> playerFactoryPtr;

}


#endif // PLAYER_HPP_INCLUDED
