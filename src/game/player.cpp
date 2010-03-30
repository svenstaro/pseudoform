 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgreSceneNode.h>
#include <OgreAnimationState.h>
#include <OgreEntity.h>
#include <OgreBone.h>
#include <OgreSkeleton.h>
#include <OgreSkeletonInstance.h>

#include "player.hpp"
#include "entity_types.hpp"

#include "portal.hpp"

#include "phys/world.hpp"

namespace game
{
    const interfaceType player::TYPE = ENTITY_PLAYER;

    player::player(
        const engine::string &name,
        const genericProp::constructParams &params):
        character(name, params),
        _cameraNode(NULL),
//        _headBone(NULL),
//        _cameraBone(NULL),
        _camPos(0, 0, 0),
        _camRot(engine::quat::IDENTITY)
//        _standAnim(NULL)
    {
        _interfaces.push_back(player::TYPE);

        loadScript("actors/sentry/sentry.prop");
//        loadScript("props/ogre.model.version.1.0/ogre.prop");
        assert(_ent && _node);

        _cameraNode = _node->createChildSceneNode();
        _cameraNode->setPosition(engine::vec3(0, 0.75, 0));
//        assert(_ent && _ent->hasSkeleton() && "player must have skeleton");
//        assert(_ent);
//        if (_ent->hasSkeleton())
//        {
//            try
//            {
//                _cameraBone = _ent->getSkeleton()->getBone("camera");
//                assert(_cameraBone);
//            }
//            catch (...)
//            {
//                engine::log("player does not have camera bone");
//            }
//
//            try
//            {
//                _headBone = _ent->getSkeleton()->getBone("head_rotate");
//                assert(_headBone);
//                _headBone->setManuallyControlled(true);
//            }
//            catch (...)
//            {
//                engine::log("player does missing head_rotate bone");
//            }
//        }
//        else
//        {
//            engine::log("player has no skeleton");
//        }

//        assert(_anims && "player must have animations");
//        if (_anims)
//        {
//            try
//            {
//                _standAnim = _anims->getAnimationState("stand");
//                assert(_standAnim);
//                _standAnim->setEnabled(true);
//                _standAnim->setLoop(true);
//            }
//            catch (...)
//            {
//            }
//        }
//        else
//        {
//            engine::log("player has no animations");
//        }

        speed = 5;
    }

    player::~player()
    {
    }

//    void player::pos(const engine::vec3 &v)
//    {
//        character::pos(v);
//        // teleported
//        _oldCamPos = _camPos;
//    }
//
//    void player::matrix(const engine::mat4 &m)
//    {
//        character::matrix(m);
//        // teleported
//        _oldCamPos = _camPos;
//    }

//    void player::pitch(float f)
//    {
//        character::pitch(f);
////        if (_headBone)
////            _headBone->pitch(engine::deg(f));
//    }

    void player::tick(engine::real dt)
    {
        using namespace engine;
        character::tick(dt);

//        if (_standAnim)
//            _standAnim->addTime(dt);

        // we need the new positions so update the node
        _node->_update(true, true);
        _camRot = _pitchNode->_getDerivedOrientation();

//        if (_headBone)
//            _headBone->setOrientation(_pitchNode->getOrientation());
//        if (_cameraBone)
//            _cameraNode->setPosition(_cameraBone->_getDerivedPosition());

        _camPos = _cameraNode->_getDerivedPosition();

        if (touchingPortal && touchingPortal->isBehindFace(_camPos))
        {
            mat4 m;
            m.makeTransform(_camPos, vec3(1, 1, 1), _camRot);

            m = touchingPortal->teleportMatrix(m);

            _camPos = m.getTrans();
            _camRot = m.extractQuaternion();
        }
    }

    entityPtr playerFactory::create(const engine::string &name) const
    {
        using namespace engine;

        playerPtr ent(new player(name, _params));

        return ent;
    }
}
