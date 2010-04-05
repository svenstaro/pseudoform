 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/signal.hpp>
#include <boost/function.hpp>

#include <algorithm>

#include <OgreSceneNode.h>
#include <OgreCamera.h>
#include <OgreMaterialManager.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderSystem.h>
#include <OgreRoot.h>
#include <OgreMeshManager.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>
#include <OgreSphere.h>
#include <OgreEntity.h>
#include <OgreRenderQueue.h>

#include <Newton.h>

#include "portal.hpp"
#include "data_dir.hpp"
#include "safe_any.hpp"
#include "world.hpp"
#include "log.hpp"
#include "entity.hpp"
#include "phys/world.hpp"
#include "entity_types.hpp"

#include "gfx/renderer.hpp"
#include "gfx/custom_params_idx.hpp"
#include "gfx/scene_clipper.hpp"

#include "level_format.pb.h"

namespace game
{

    namespace
    {
        float maxf(float a, float b)
        {
            return a < b ? b : a;
        }
    }

    portal::portalList portal::_allPortals;

    const interfaceType portal::TYPE = ENTITY_PORTAL;

    portal::portal(
        const engine::string &name,
        const genericProp::constructParams &params)://, engine::gfx::renderer &renderer):
        genericProp(name, params),
        _camera(NULL),
        _stencilPass(NULL),
        _depthPass(NULL),
        _clearDepthPass(NULL),
        _detector(new detector(params.world))
    {
        _interfaces.push_back(portal::TYPE);

        // load from a script
//        loadScript("portal/frame.prop");

        // add ourself to the list
        _allPortals.push_back(this);

        // Create our camera we'll be using
        _camera = params.parentNode->getCreator()->createCamera(_name + "::_camera");

        try
        {
            _partialProxy = (Ogre::MeshPtr)Ogre::MeshManager::getSingleton().load(
                "portal/square_portal_plane.mesh",
                Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
            _fullProxy = (Ogre::MeshPtr)Ogre::MeshManager::getSingleton().load(
                "portal/square_portal_full.mesh",
                Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
        }
        catch (...)
        {
            // Use the if below
        }
        if (!_partialProxy.get() || !_fullProxy.get())
        {
            engine::log("Could not load portal proxies, portal rendering will be disabled");
        }

        try
        {
            Ogre::MaterialPtr m = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().load(
                "portal", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
            _stencilPass = m->getTechnique(0)->getPass("stencil");
            _depthPass = m->getTechnique(0)->getPass("depth");
            _clearDepthPass = m->getTechnique(0)->getPass("clearDepth");
        }
        catch (...)
        {
        }
        if (!_stencilPass || !_depthPass || !_clearDepthPass)
        {
            engine::log("Could not load portal shaders, portal rendering will be disabled");
        }
    }

    portal::~portal()
    {
        if (_camera)
            _camera->getSceneManager()->destroyCamera(_camera);
        _camera = NULL;

        // Remove ourselves from the list
        _allPortals.erase(std::find(_allPortals.begin(), _allPortals.end(), this));

        assert(_connectedToThis.empty());
        // No one should be connected to us at this point
    }

    void portal::willChangeTo(world *w)
    {
        if (_parent != w) // Removed from world
        {
            BOOST_FOREACH(portal *i, _connectedToThis)
                i->_connection.reset(); // Remove connection TO us so we can be destroyed
            _connectedToThis.clear();

            for (copyMap::iterator i = _copies.begin(); i != _copies.end(); ++i)
            {
                _destroyCopy(*i);
            }
            _copies.clear();

            //engine::log("changing to new world");
        }
    }

    portal::detector::detector(const engine::phys::world &w):
        engine::phys::body(w)
    {
        collision(engine::phys::box(w, engine::vec3(1, 1, 0.1)));
        // We want to be dynamic
        mass(1);
        // And never fall asleep
        autoSleep(false);
        // But we don't need history to record b/c it'll waste memory
        _recordHistory = false;
        // And we want to be swept
        swept(true);

        //! set as trigger volume maybe?
//        NewtonCollisionSetAsTriggerVolume(*collision(), 1);
    }

    void portal::detector::forceTorque(float dt)
    {
        touches.clear();
//        assert(_parent);
//        engine::vec3 pPos = _parent->pos(), thisPos = pos();
//        if ((pPos - thisPos).squaredLength() > 0.25 * 0.25)
//        {
//            matrix(_parent->matrix());
//            vel(engine::vec3(0, 0, 0));
//        }
//        else
//        {
//            vel(pPos - thisPos); // move to our parent's position
//            rot(_parent->rot());
//        }

        //pos(_parent->pos());
        //vel(_parent->vel());
        //engine::vec3 p = pos();
        ///matrix(_parent->matrix());
        //vel(pos()
        //vel(_parent->vel());
        //omega(_parent->omega());
    }
//
    void portal::detector::processContacts(engine::phys::body &o,
        const engine::phys::contactList &contacts, engine::phys::contactList &rejections, float dt)
    {
        engine::phys::body::processContacts(o, contacts, rejections, dt);

//        assert(_parent);

        using namespace engine;
        using namespace engine::phys;

        contactList::const_iterator i = contacts.begin();
        for (; i != contacts.end(); ++i)
        {
            //contact &c = *(*i); // it's a shared pointer
            rejections.push_back(*i);
        }

        // don't bother with portals not belonging to a world
//        if (!_parent->parent())
//            return;

        if (o.mass() < 0.01) // Forget about statics
            return;

        if (o.misc.empty())
            return;

        entity *ent = SAFE_ANY_CAST_T(entity*, o.misc, NULL);
        // Either NULL or not in a world
        if (!ent || !ent->parent())
            return;

        // Don't mess with portals for now
        if (ent->hasInterface(portal::TYPE))
            return;

        // Make sure something funky isn't going on
        assert(ent->hasInterface(genericProp::TYPE));

        genericProp *gp = static_cast<genericProp*>(ent);

        // GenericProp inherits from body... should be the same address
//        assert(gp == &o);

        assert(gp);

        touches.push_back(gp);

//        portal::copyMap::iterator j = _parent->_copies.find(gp);
//        if (j == _parent->_copies.end())
//        {
//            // if it's a new object, we need to make sure it's going into
//            // the portal, and not just behind it
//            if (_parent->isBehindFace(gp->pos()))
//                return;
//
//            _touches.push_back(gp);
//
//            // a bit of a hack, but generic props (such as the player)
//            // sometimes need to know what portal they're currently touching
////            static_cast<genericProp*>(ent)->touchingPortal = _parent;
////
////            o.contactClipPlane = _parent->plane();
////
////            // not created, create it
////            copyInfo &info = _parent->_copies[&o];
////            info.timeSinceContact = 0;
////
////            info.gp = ENTITY_CAST(genericProp, ent->clone(ent->name() + "::_copy"));
////            ent->parent()->add(info.gp);
////
////            info.joint = new portalJoint(*info.gp, o);
////            info.joint->relativeScale = _parent->relativeScale().x; // Y should be the same
////
////            info.gp->matrix(_parent->teleportMatrix(o.matrix()));
////            info.gp->vel(_parent->teleportDir(o.vel()));
////            info.gp->scale(_parent->relativeScale() * o.scale());
////
////            // make sure we don't get duplicates of the copy
////            _parent->connection()->_detector->ignoreList.push_back(info.gp.get());
//        }
//        else
//        {
//            // tick it
//            j->second.timeSinceContact -= dt;
//        }

//        vec3 fwd = _parent->rot().zAxis();
//        vec3 oPos = o.pos();
//        vec3 thisPos = _parent->pos();
//        vec3 oVel = o.vel();
//        vec3 thisVel = _parent->vel();
//        if ((oPos - thisPos).dotProduct(fwd) < 0) // in back of us
//        {
//            ///log("in back of");
//            // check the past
//            //if ((o.latestHistory().p - pos()).dotProduct(fwd) > 0)
//            const float TIME_COEF = 1;
//            vec3 relVel = oVel - thisVel;
//            vec3 oldOPos = oPos + (-relVel * TIME_COEF);
//            if ((oldOPos - thisPos).dotProduct(fwd) >= 0)
//            {
//                // in front of us
//                ///log("candidate for teleportation");
//                ///_objToTeleport = &o;
//                _parent->_teleport(o, relVel);
//
//                // register a teleportation
////                _parent->_safeFuncs.push_back(boost::bind(
////                    &portal::_teleport, _parent, boost::ref(o), relVel));
//            }
//        }
//        else
//        {
//            ///log("in front of");
//        }
    }

//    void portal::detector::tick(float dt)
//    {
//        using namespace engine;
//        touchList finished;
//        BOOST_FOREACH(genericProp *gp, _touches)
//        {
//            if (std::find(finished.begin(), finished.end(), gp) != finished.end())
//                // already processed, ignore
//                continue;
//            finished.push_back(gp);
//
//            // a bit of a hack, but generic props (such as the player)
//            // sometimes need to know what portal they're currently touching
//            gp->touchingPortal = _parent;
//            gp->contactClipPlane = _parent->plane();
//            gp->vel(gp->vel() - _parent->vel());
//
//            // not created, create it
//            copyInfo &info = _parent->_copies[gp];
//            info.timeSinceContact = 0;
//
//            info.gp = ENTITY_CAST(genericProp, gp->clone(gp->name() + "::_portalCopy"));
//            gp->parent()->add(info.gp);
//            info.gp->useSkeletonFrom(*gp);
//
////            info.gp->mass(info.gp->mass() * 0.5);
////            gp->mass(gp->mass() * 0.5);
//
//            info.joint = new portalJoint(*info.gp, *gp);
//            info.joint->relativeScale = _parent->relativeScale().x; // Y should be the same
//            //std::cout << "rel scale " << _parent->relativeScale();
//
//            info.gp->matrix(_parent->teleportMatrix(gp->matrix()));
//            info.gp->vel(_parent->teleportDir(gp->vel()));
//            info.gp->omega(gp->omega());
//            info.gp->scale(_parent->relativeScale() * gp->scale());
//
//            info.wasAutoSleep = gp->autoSleep();
//            gp->autoSleep(false);
//
//            // make sure we don't get duplicates of the copy
//            _parent->connection()->_detector->ignoreList.push_back(info.gp.get());
//        }
//        _touches.clear();
//    }

    bool portal::isVisible(Ogre::Camera *cam) const
    {
        // Test if we're visible

        engine::vec3 p = pos();

        if (!cam->isVisible(aabb()))
            return false; // not visible

        if (!isInside(cam->getDerivedPosition()) && normal().dotProduct(cam->getDerivedPosition() - p) < 0)
            return false; // facing away

//        float facingDev = std::acos(cam->getDerivedDirection().dotProduct(normal()));
//        if (engine::rad(facingDev).valueDegrees() < cam->getFOVy().valueDegrees() * 0.5)
//            return false;

        return true;
    }

    bool portal::isBehindFace(const engine::vec3 &p) const
    {
        engine::vec4 us = plane();
        engine::vec3 n(us.x, us.y, us.z);
        if (n.dotProduct(p) + us.w < 0)
            return true;
        return false;
    }

    bool portal::isWithinFace(const engine::vec3 &p) const
    {
        engine::vec3 local = matrix().inverse() * p;
        engine::vec3 s = scale();
        // check if the point is within our square
        return local.x >= s.x * -0.5 && local.x <= s.x * 0.5 &&
            local.y >= s.y * -0.5 && local.y <= s.y * 0.5;
    }

    float portal::distFromFace(const engine::vec3 &p) const
    {
        return std::abs(normal().dotProduct(p - pos()));
    }

    void portal::_render(Ogre::Camera *cam, Ogre::Viewport *vp, unsigned iter)
    {
        if (!_partialProxy.get() || !_fullProxy.get() || !_stencilPass)
            return;

        if (_parent == NULL)
            // No world, don't render
            return;

        assert(_camera);

        if (!isVisible(cam))
            return;

        _camera->setFOVy(cam->getFOVy());
        _camera->setAspectRatio(cam->getAspectRatio());
        _camera->setNearClipDistance(cam->getNearClipDistance());
        _camera->setFarClipDistance(cam->getFarClipDistance());
        _camera->setPolygonMode(cam->getPolygonMode());

        using namespace engine;

        // Save original position/orientation
        vec3 camPos = cam->getDerivedPosition();
        quat camRot = cam->getDerivedOrientation();

        _camera->setPosition(camPos);
        _camera->setOrientation(camRot);

        Ogre::RenderSystem *rs = Ogre::Root::getSingleton().getRenderSystem();
        rs->_setViewport(vp);

        // Clear depth and possibly stencil if this is the first render
//        if (iter == 1)
//            rs->clearFrameBuffer(Ogre::FBT_DEPTH | Ogre::FBT_STENCIL);
//        else
//            rs->clearFrameBuffer(Ogre::FBT_DEPTH);
        if (iter == 1)
            rs->clearFrameBuffer(Ogre::FBT_STENCIL);

        // re-render depth buffer for ourselves
//        string oldMS = vp->getMaterialScheme();
//
//        vp->setMaterialScheme("depth");
//        Ogre::MaterialManager::getSingleton().setActiveScheme(vp->getMaterialScheme());
//
//        // render
//        _camera->getSceneManager()->_renderScene(_camera, vp, false);
//
//        vp->setMaterialScheme(oldMS);

        mat4 viewMat = _camera->getViewMatrix(true);
        mat4 projMat = _camera->getProjectionMatrixRS();

//        if (vp->getTarget()->requiresTextureFlipping())
//        {
//            // invert transformed y
//            projMat[1][0] = -projMat[1][0];
//            projMat[1][1] = -projMat[1][1];
//            projMat[1][2] = -projMat[1][2];
//            projMat[1][3] = -projMat[1][3];
//        }

        rs->_setViewMatrix(viewMat);
        rs->_setProjectionMatrix(projMat);

//        _camera->getSceneManager()->_setPass(_depthPass, true, false);

        // render other portals depths to occlude
//        BOOST_FOREACH(portal *i, _allPortals)
//        {
//            if (i != this)
//                i->renderSurface();
//        }

        _camera->getSceneManager()->_setPass(_stencilPass, true, false);
//        _stencilClearDepthPass->getVertexProgramParameters()->setNamedConstant("wvpMat",
//            projMat * viewMat * worldMat);

        rs->setStencilCheckEnabled(true);
        rs->setStencilBufferParams(Ogre::CMPF_EQUAL, iter - 1, 0xFFFFFFFF, Ogre::SOP_KEEP, Ogre::SOP_KEEP,
            Ogre::SOP_INCREMENT, false);

        renderSurface(camPos);

        //        rs->setStencilCheckEnabled(true);
        rs->setStencilBufferParams(Ogre::CMPF_EQUAL, iter, 0xFFFFFFFF, Ogre::SOP_KEEP, Ogre::SOP_KEEP,
            Ogre::SOP_KEEP, false);

        _camera->getSceneManager()->_setPass(_clearDepthPass, true, false);
        // Clear the depth where we are so the other scene can be rendered
        renderSurface(camPos);

        // Rotate our camera as appropriate
        mat4 camMat;
        camMat.makeTransform(camPos, vec3(1, 1, 1), camRot);

        camMat = teleportMatrix(camMat);

        _camera->setPosition(camMat.getTrans());
        _camera->setOrientation(camMat.extractQuaternion());

        // don't clear depth buffer, it was cleared by stencilClearDepth
        //!rs->clearFrameBuffer(Ogre::FBT_DEPTH);

        // clip anything behind the portal
//        vec4 cp = _connection->plane();
//        _camera->enableCustomNearClipPlane(Ogre::Plane(vec3(cp.x, cp.y, cp.z), cp.w));

        /*! we need to clip stuff behind the portal
            the problem is that we need the same projection matrix
            so that the depth buffer can be shared properly...
            thus, we need to implement clipping manually in the shaders
        */
//        {
//            vec3 n = connection()->normal();
//            Ogre::Plane clipPlane(n, connection()->pos() + n * -0.005);
//            _camera->enableCustomNearClipPlane(clipPlane);
//        }

//        vp->setMaterialScheme("Default");
        Ogre::MaterialManager::getSingleton().setActiveScheme(vp->getMaterialScheme());

        {
            vec4 p(0, 0, 0, 1);
            if (!isInside(camPos))
                p = connection()->plane();

            engine::gfx::sceneClipper clipper(p,
                _camera->getSceneManager()->getRenderQueue());
            // render
            _camera->getSceneManager()->_renderScene(_camera, vp, false);
        }

        // disable the clipping
//        _camera->disableCustomNearClipPlane();

//        log("wtf?");

        rs->setStencilCheckEnabled(false);

        if (iter < 5)
            renderAllPortals(_camera, vp, iter + 1);
//            BOOST_FOREACH(portal *p, _allPortals)
//            {
////                if (p == this)
////                    continue;
//
//                p->_render(_camera, vp, iter + 1);
//            }

//        if (iter < 10 & connection() != this)
//        {
//            _render(_camera, vp, iter + 1);
//        }
    }

    void portal::pos(const engine::vec3 &v)
    {
        genericProp::pos(v);
        _detector->pos(v);
    }

    void portal::rot(const engine::quat &q)
    {
        genericProp::rot(q);
        _detector->rot(q);
    }

    void portal::matrix(const engine::mat4 &m)
    {
        genericProp::matrix(m);
        _detector->matrix(m);
    }

    engine::vec3 portal::pos() const
    {
        return genericProp::pos();
    }

    engine::quat portal::rot() const
    {
        return genericProp::rot();
    }

    engine::mat4 portal::matrix() const
    {
        return genericProp::matrix();
    }

    bool portal::isInside(const engine::vec3 &p) const
    {
        float dist = distFromFace(p);
//        if (isWithinFace(p) && (isBehindFace(p) && dist < 1.6 || dist < 0.2))
        if (isWithinFace(p) && std::abs(dist) < 0.09)
            return true;
        return false;
    }

    void portal::getRenderOp(const engine::vec3 &p, bool &depthCheck, Ogre::RenderOperation &rop) const
    {
        if (isInside(p))
        {
            depthCheck = false;
            _fullProxy->getSubMesh(0)->_getRenderOperation(rop);
        }
        else
        {
            depthCheck = true;
            _partialProxy->getSubMesh(0)->_getRenderOperation(rop);
        }
    }

    void portal::renderSurface(const engine::vec3 &camPos) const
    {
        using namespace engine;

        Ogre::RenderSystem *rs = Ogre::Root::getSingleton().getRenderSystem();

        mat4 worldMat = matrixWithScale();
        rs->_setWorldMatrix(worldMat);

        Ogre::RenderOperation rop;
        bool depthCheck = true;
        getRenderOp(camPos, depthCheck, rop);

        if (!depthCheck)
            rs->_setDepthBufferFunction(Ogre::CMPF_ALWAYS_PASS);

        rs->_beginFrame();
        {
            rs->_render(rop);
        }
        rs->_endFrame();
    }

    engine::mat4 portal::matrixWithScale() const
    {
        using namespace engine;
        mat4 worldMat;
        vec3 scaleNoZ = scale();
        scaleNoZ.z = 1;
        worldMat.makeTransform(pos(), scaleNoZ, rot());
        return worldMat;
    }

    engine::vec3 portal::relativeScale() const
    {
        // 2 * x = 1
        // x = 1 / 2
        engine::vec3 ret = connection()->scale() / scale();
        // Z should be equal to X and Y (take average just in case)
        ret.z = (ret.x + ret.y) * 0.5;
        return ret;
    }

//    float portal::radius() const
//    {
//        engine::vec3 s = scale();
//        return maxf(s.x, s.y);
//    }

    namespace
    {
        struct sortByDist
        {
            engine::vec3 camPos;
            sortByDist(const engine::vec3 &camPos):
                camPos(camPos)
            {
            }

            bool operator()(portal *a, portal *b)
            {
                // Furthest portals go in the front
                return a->pos().squaredDistance(camPos) > b->pos().squaredDistance(camPos);
            }
        };
    }

    void portal::renderAllPortals(Ogre::Camera *cam, Ogre::Viewport *vp, unsigned iter)
    {
        // depth-sort the portals
        portalList tempList = _allPortals;
        // sort a copy of the list because we'll have recursive calls here
        // and each one will sort the portal list, resulting in invalid
        // sorting later on
        std::sort(tempList.begin(), tempList.end(), sortByDist(cam->getDerivedPosition()));

        BOOST_FOREACH(portal *p, tempList)
            p->_render(cam, vp, iter);
    }

//    void portal::detector::contactsCriticalSection()
//    {
//        if (_objToTeleport)
//        {
//            _parent->_teleport(*_objToTeleport, _objToTeleport->vel());
//            _objToTeleport = NULL;
//        }
//    }

    void portal::_teleport(engine::phys::body &obj, const engine::vec3 &vel)
    {
        obj.vel(teleportDir(vel));
        obj.matrix(teleportMatrix(obj.matrix()));
    }

    engine::vec3 portal::teleportDir(const engine::vec3 &v)
    {
        using namespace engine;
        vec3 ret = rot().Inverse() * v;

        const quat ROT180(deg(180), vec3(0, 1, 0));
        quat connectionRot = connection()->rot() * ROT180;

        return (connectionRot * ret) * relativeScale();
    }

    engine::mat4 portal::teleportMatrix(const engine::mat4 &m)
    {
        using namespace engine;
        mat4 thisMat = matrix();

        const quat ROT180(deg(180), vec3(0, 1, 0));
        mat4 connectionMat;
        connectionMat.makeTransform(connection()->pos(), vec3(1, 1, 1), connection()->rot() * ROT180);

        mat4 rel = thisMat.inverse() * m;

        // Do the scaling in local space
        rel.setTrans(rel.getTrans() * relativeScale());

        return connectionMat * rel;
    }

    engine::vec3 portal::teleportPos(const engine::vec3 &v)
    {
        using namespace engine;
        mat4 thisMat = matrix();

        const quat ROT180(deg(180), vec3(0, 1, 0));
        mat4 connectionMat;
        connectionMat.makeTransform(connection()->pos(), vec3(1, 1, 1), connection()->rot() * ROT180);

        vec3 rel = thisMat.inverse() * v;

        // do the scaling in local space
        rel *= relativeScale();

        return connectionMat * rel;
    }

    void portal::connection(const portalPtr &p)
    {
        if (_connection && _connection != p)
        {
            _connection->_connectedToThis.remove(this);
        }

        _connection = p;
        if (_connection)
            _connection->_connectedToThis.push_back(this);
    }

    void portal::scale(const engine::vec3 &s)
    {
        genericProp::scale(s);

        if (s.x >= 0.01 && s.y >= 0.01 && s.z >= 0.01)
        {
            // GenericProp will update our node and our body
            // but we must update our detector manually
            _detector->scale(s);
        }
    }

    void portal::_destroyCopy(copyMap::value_type &i)
    {
        delete i.second.joint;

        connection()->_detector->ignoreList.remove(i.second.gp.get());

        // delete the copy object
        _parent->remove(i.second.gp->name());
        i.second.gp.reset();

        genericProp *gp = i.first;
        gp->clipPlane(engine::vec4(0, 0, 0, 1));
        gp->touchingPortal = NULL;
    }

    void portal::reset()
    {
        genericProp::reset();

        // Remove all duplicates

        assert(_parent && "cannot reset() outside of world");

        for (copyMap::iterator i = _copies.begin(); i != _copies.end(); ++i)
        {
            _destroyCopy(*i);
        }
        _copies.clear();
    }

    void portal::tick(engine::real dt)
    {
        using namespace engine;

        genericProp::tick(dt);

        if (!_parent)
            return;

//        _detector->tick(dt);

//        BOOST_FOREACH(const safeFunc &i, _safeFuncs)
//            i();
//        _safeFuncs.clear();

//        engine::vec3 thisPos = pos(), detectorPos = _detector->pos();
//        if ((detectorPos - thisPos).squaredLength() > 0.1 * 0.1) // greater than 10 cm apart?
//        {
//            _detector->vel(engine::vec3(0, 0, 0));
//            _detector->omega(engine::vec3(0, 0, 0));
//        }
//        else
//        {
//            _detector->vel(vel());
//            _detector->omega(omega());
//        }

        // loop through all the copies, age them, and teleport those
        // which are not touching us anymore
        vec4 ourPlane = plane();
        vec4 connectionPlane = connection()->plane();
        vec4 noPlane(0, 0, 0, 1);

        std::map<genericProp*, genericProp*> touching;

//        for (NewtonJoint *contactJoint = NewtonBodyGetFirstContactJoint(*_detector);
//            contactJoint != NULL; contactJoint = NewtonBodyGetNextContactJoint(*_detector, contactJoint))
//        {
//            genericProp *gp = NULL;
//            {
//                NewtonBody *b0 = NewtonJointGetBody0(contactJoint);
//                NewtonBody *b1 = NewtonJointGetBody1(contactJoint);
//                // detector body
//                NewtonBody *d = (NewtonBody*)(*_detector);
//                // get the body that's not the detector
//                NewtonBody *fin = (b0 == d ? b1 : b0);
////                std::cout << "found something: " << d << " , " << fin << "\n";
//
//                phys::body *body = phys::bodyCast(fin);
//
//                if (!body)
//                    continue;
//
////                std::cout << "found something: " << d << " , " << fin << "\n";
//
//                if (body->mass() < 0.01) // forget about statics
//                    continue;
//
//                std::cout << "found something dynamic: " << d << " , " << fin << "\n";
//
//                if (body->misc.empty())
//                    continue;
//
//                entity *ent = SAFE_ANY_CAST_T(entity*, body->misc, NULL);
//                // either NULL or not in a world
//                if (!ent || !ent->parent())
//                    continue;
//
//                // don't mess with portals for now
//                if (ent->hasInterface(portal::TYPE))
//                    continue;
//
//                // make sure something funky isn't going on
//                assert(ent->hasInterface(genericProp::TYPE));
//
//                gp = static_cast<genericProp*>(ent);
//            }
//            assert(gp);

//        if (_detector->touches.empty())
//            std::cout << "none\n";
//        else
        BOOST_FOREACH(genericProp *gp, _detector->touches)
        {
            touching[gp] = gp;

            // a bit of a hack, but generic props (such as the player)
            // sometimes need to know what portal they're currently touching
            gp->touchingPortal = this;
            gp->contactClipPlane = ourPlane;

            // V this seems wrong V
//            gp->vel(gp->vel() - _parent->vel());

            copyMap::iterator copyIter = _copies.find(gp);
            if (copyIter == _copies.end())
            {
                // Not created, create it
                copyInfo &info = _copies[gp];

                info.gp = ENTITY_CAST(genericProp, gp->clone(gp->name() + "::_portalCopy"));
                gp->parent()->add(info.gp);
                info.gp->useSkeletonFrom(*gp);

                info.joint = new portalJoint(*info.gp, *gp);
                info.joint->relativeScale = relativeScale().x; // Y should be the same

                info.gp->matrix(teleportMatrix(gp->matrix()));
                info.gp->vel(teleportDir(gp->vel() - vel()));
                info.gp->omega(gp->omega());
                info.gp->scale(relativeScale() * gp->scale());

                // Make sure we don't get duplicates of the copy
                connection()->_detector->ignoreList.push_back(info.gp.get());
            }
            else
            {
                genericProp *gp = copyIter->first;
                gp->clipPlane(ourPlane);

                copyIter->second.gp->clipPlane(connectionPlane);

                // Calibrate the copy in case the portals moved or something
                mat4 ideal = teleportMatrix(gp->matrix());
                mat4 actual = copyIter->second.gp->matrix();

                // Check position, no more than 0.001m offset
                // Check rotations, no more than 1 degree off (0.999 cosine)

                vec3 idealPos = ideal.getTrans();
                vec3 actualPos = actual.getTrans();

                quat idealRot = ideal.extractQuaternion();
                quat actualRot = actual.extractQuaternion();

                if ((idealPos - actualPos).squaredLength() > 0.001 * 0.001 ||
                    idealRot.xAxis().dotProduct(actualRot.xAxis()) < 0.9999 ||
                    idealRot.yAxis().dotProduct(actualRot.yAxis()) < 0.9999)
                    // No need to check Z, XY can be used to calculate it
                {
                    // Must recalibrate...
                    copyIter->second.gp->matrix(ideal);
                    //i->second.gp->vel(teleportDir(gp->vel()));
                }
            }
        }

        std::vector<copyMap::iterator> toRemove;
        for (copyMap::iterator i = _copies.begin(); i != _copies.end(); ++i)
        {
            if (touching.find(i->first) == touching.end())
            {
                _destroyCopy(*i);

                toRemove.push_back(i);

                genericProp *gp = i->first;

                // If the body is behind us, then we need to teleport it
                if (isBehindFace(gp->pos()))
                {
                    gp->matrix(teleportMatrix(gp->matrix()));
                    gp->vel(teleportDir(gp->vel()));
                    gp->scale(gp->scale() * relativeScale());
                }
            }
        }
        BOOST_FOREACH(copyMap::iterator &i, toRemove)
            _copies.erase(i);

        _detector->matrix(matrix());
//        _detector->touches.clear();
    }

    engine::vec4 portal::plane() const
    {
        engine::vec3 n = rot().zAxis();
        Ogre::Plane p(n, pos() + n * -0.04 * scale().z);
        return engine::vec4(p.normal.x, p.normal.y, p.normal.z, p.d);
    }

    engine::vec3 portal::normal() const
    {
        return rot().zAxis();
    }

    void portal::load(const levelFormat::entity &desc)
    {
        genericProp::load(desc);

        using namespace engine;

        if (!desc.has_portal_info())
        {
            log("Portal " + _name + " has no portal_info");
            return;
        }

        const levelFormat::portalInfo &info = desc.portal_info();

        if (info.has_connection() && _parent)
        {
            portalPtr p = ENTITY_CAST(portal, _parent->get(info.connection()));
            connection(p);
        }
    }

    void portal::save(levelFormat::entity &desc)
    {
        genericProp::save(desc);

        levelFormat::portalInfo *info = desc.mutable_portal_info();

        if (_connection)
            info->set_connection(_connection->name());
    }

    entityPtr portalFactory::create(const engine::string &name) const
    {
        using namespace engine;

        portalPtr ent(new portal(name, _params));

        return ent;
    }
}
