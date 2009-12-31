 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <sstream>
#include <map>

#include <boost/foreach.hpp>

#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreSceneNode.h>
#include <OgreSubMesh.h>
#include <OgreSceneManager.h>
#include <OgreRenderQueue.h>
#include <OgreAnimationState.h>
#include <OgreMeshManager.h>
#include <OgreStringConverter.h>

#include "generic_prop.hpp"
#include "log.hpp"
#include "data_dir.hpp"

#include "safe_any.hpp"
#include "types.hpp"
#include "txml/txml.hpp"
#include "entity_types.hpp"

#include "protobuf/level_format.pb.h"

#include "phys/body.hpp"
#include "phys/mesh.hpp"
#include "phys/collision.hpp"
#include "phys/world.hpp"

#include "snd/system.hpp"

#include "error_mesh.hpp"

#include "gfx/renderer.hpp"
#include "gfx/custom_params_idx.hpp"

namespace game
{

    namespace
    {

        void setMaterialName(Ogre::Entity *ent, const engine::string &mat)
        {
            if (mat.empty())
            {
                // if empty, use the default materials
                for (size_t i = 0; i < ent->getNumSubEntities(); ++i)
                {
                    Ogre::SubEntity *sub = ent->getSubEntity(i);
                    sub->setMaterialName(sub->getSubMesh()->getMaterialName());
                }
            }
            else
            {
                ent->setMaterialName(mat);
            }
        }
    }

//    const unsigned CLIP_PLANE_IDX = 256;
//    const unsigned USE_CLIP_PLANE_IDX = 257;

    const interfaceType genericProp::TYPE = ENTITY_GENERIC_PROP;

    genericProp::genericProp(const engine::string &n,
        const constructParams &params):
        entity(n),
        engine::phys::body(params.world),
        _node(NULL),
        _ent(NULL),
        _anims(NULL),
        _origScale(1, 1, 1),
        _visible(true),
        _soundSys(params.soundSys),
        touchingPortal(NULL)
    {
        _interfaces.push_back(genericProp::TYPE);

        this->misc = (entity*)this;

        _node = params.parentNode->createChildSceneNode(_name + "::_node");
    }

    genericProp::genericProp(const engine::string &n, const genericProp &o):
        entity(n),
        engine::phys::body(o._world),
        _node(NULL),
        _ent(NULL),
        _anims(NULL),
        _origScale(1, 1, 1),
        _visible(true),
        _soundSys(o._soundSys),
        touchingPortal(NULL)
    {
        _interfaces.push_back(genericProp::TYPE);

        this->misc = (entity*)this;

        fromGenericProp(o);
    }

    genericProp::~genericProp()
    {
        _destroyEnt();
        _destroyNode();
    }

    void genericProp::processContacts(engine::phys::body &o,
        const engine::phys::contactList &contacts, engine::phys::contactList &rejections, float dt)
    {
        using namespace engine;
        using namespace engine::phys;

        body::processContacts(o, contacts, rejections, dt);

        if (o.misc.empty())
            return;

//        contactList::const_iterator i = contacts.begin();
//        for (; i != contacts.end(); ++i)
//        {
//            contact &c = *(*i); // it's a shared pointer
//
//            float vol = c.normalSpeed / 10.0;
//            if (vol > 0.1)
//            {
////                using namespace engine::snd;
////
////                soundPtr s = _soundSys.play("media/sounds/clack10.ogg", true);
////                s->volume(vol);
//                colSnd s = { "media/sounds/clack10.ogg", c.position, vol };
//                _colSnds.push_back(s);
//
//                break;
//            }
//        }
    }

    void genericProp::reset()
    {
        using namespace engine;

        entity::reset();
        clearHistory();

        vel(vec3(0, 0, 0));
        omega(vec3(0, 0, 0));
    }

    void genericProp::setMaterial(const engine::string &str)
    {
        assert(_ent);
        _origMaterial = str;
        setMaterialName(_ent, str);
    }

    void genericProp::loadScript(const engine::string &fn)
    {
        using namespace engine;

        txml::document doc;
        doc.LoadFile((engine::DATA_DIR + fn).c_str());
        if (doc.Error())
        {
            engine::log("genericProp::loadScript script \"" + fn + "\" is invalid");
            return;
        }

        _docFileName = fn;

        txml::element *rootElem = doc.RootElement();

        txml::node *elemNode = NULL;
        txml::element *elemChild = NULL;

        if ((elemNode = rootElem->FirstChild("scale")) && (elemChild = elemNode->ToElement()))
        {
            vec3 v(elemChild->attrib("x", 1.0), elemChild->attrib("y", 1.0),
                elemChild->attrib("z", 1.0));

            _origScale = v;
            _node->setScale(_origScale);
        }

        if ((elemNode = rootElem->FirstChild("gfx")) && (elemChild = elemNode->ToElement()))
        {
            string mesh = elemChild->attrib<engine::string>("mesh");
            string group = elemChild->attrib<engine::string>("group");
            string skeleton = elemChild->attrib<engine::string>("skeleton");
            string material = elemChild->attrib<engine::string>("material");

            // remove previously parsed entities
            _destroyEnt();

            Ogre::MeshPtr meshPtr;
            if (!mesh.empty())
            {
                try
                {
                    meshPtr = Ogre::MeshManager::getSingleton().load(
                        mesh, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
                }
                catch(...)
                {
                    log("entity \"" + _name + "\" cannot load mesh \"" + mesh + "\"");
                }
            }

            if (mesh.empty() || !meshPtr.get())
            {
                mesh = engine::ERROR_MESH;
                try
                {
                    meshPtr = Ogre::MeshManager::getSingleton().load(
                        mesh, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
                }
                catch(...)
                {
                    goto skipGfx;
                }
            }

            // make sure tangent vectors are built
//            try
//            {
//                unsigned short src, dest;
//                if (!meshPtr->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
//                {
//                    meshPtr->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
//                    // Second mode cleans mirrored / rotated UVs but requires quality models
//                    //meshPtr->buildTangentVectors(Ogre::VES_TANGENT, src, dest, true, true);
//                }
//            }
//            catch (...)
//            {
//                log("could not generate tangents for entity \"" + _name + "\", will not be normal-mapped");
//            }

            // try to load a skeleton if there
            if (meshPtr.get())
            {
                if (!skeleton.empty())
                {
                    try
                    {
                        meshPtr->setSkeletonName(skeleton);
                    }
                    catch(...)
                    {
                        log("could not set skeleton \"" + skeleton + "\" on mesh \"" + mesh + "\"");
                    }
                }
            }

            _ent = _node->getCreator()->createEntity(_name + "_ent", mesh);
            _ent->setVisibilityFlags(gfx::MAIN_GROUP);
            _ent->refreshAvailableAnimationState();
            _ent->setVisible(_visible);
            _node->attachObject(_ent);

            for (size_t i = 0; i < _ent->getNumSubEntities(); ++i)
            {
                Ogre::SubEntity *sub = _ent->getSubEntity(i);
                sub->setCustomParameter(engine::gfx::OBJECT_CLIP_PLANE_IDX, engine::vec4(0, 0, 0, 1));
            }

            if (!group.empty())
            {
                // assign groups
                if (group == "primary")
                    _ent->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1);
                else if (group == "secondary")
                    _ent->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);
            }

            if (!material.empty())
            {
//                _origMaterial = material;
                setMaterialName(_ent, material);
            }

            _anims = _ent->getAllAnimationStates();
            if (_anims)
            {
                for (txml::node *animNode = elemNode->FirstChild("anim");
                    animNode; animNode = elemNode->IterateChildren("anim", animNode))
                {
                    txml::element *e = NULL;
                    if (!(e = animNode->ToElement()))
                        continue;

                    string name = e->attrib<string>("name");
                    bool loop =  e->attrib("loop", true);

                    if (!_anims->hasAnimationState(name))
                    {
                        log("no animation named \"" + name + "\" in mesh \"" + mesh);
                        continue;
                    }

                    Ogre::AnimationState *anim = _anims->getAnimationState(name);
                    anim->setEnabled(true);
                    anim->setWeight(1.0);
                    anim->setLoop(loop);
                }
            }

        }

        // in case the mesh failed to load
        skipGfx:

        if ((elemNode = rootElem->FirstChild("phys")) && (elemChild = elemNode->ToElement()))
        {
            string type = elemChild->attrib<engine::string>("type");
            string mesh = elemChild->attrib<engine::string>("mesh");

            vec3 size(elemChild->attrib("x", 1.0), elemChild->attrib("y", 1.0),
                elemChild->attrib("z", 1.0));

            size *= _origScale; // scale it up/down

            using namespace engine::phys;

            collisionPtr col;

            if (type == "convex" && !mesh.empty()) col.reset(new convexHullCollision(_world, mesh, _origScale));
            else if (type == "concave" && !mesh.empty()) col.reset(new concaveHullCollision(_world, mesh, _origScale));
            else if (type == "tree" && !mesh.empty()) col.reset(new treeCollision(_world, mesh, _origScale));
            else if (type == "box" || type == "cube") col = box(_world, size);
            else if (type == "ball" || type == "sphere") col = ball(_world, size);
            else if (type == "cylinder") col = cylinder(_world, size.x, size.y);
            else if (type == "cone") col = cone(_world, size.x, size.y);
            else if (type == "capsule") col = capsule(_world, size.x, size.y);

            if (col)
                collision(col);
            else
                engine::log("physics-based entity " + _name + " has no collision");
        }
    }

    void genericProp::_destroyEnt()
    {
        if (_node && _ent)
        {
            _node->detachObject(_ent);
        }

        if (_ent)
        {
            _ent->_getManager()->destroyEntity(_ent);
            _ent = NULL;
        }
    }

    void genericProp::_destroyNode()
    {
        if (_node)
        {
            _node->getParentSceneNode()->removeAndDestroyChild(_node->getName());
            _node = NULL;
        }
    }

    void genericProp::tick(engine::real dt)
    {
        entity::tick(dt);

        // advance our enabled animations, if we have any
        if (_anims)
        {
            for (Ogre::ConstEnabledAnimationStateIterator i = _anims->getEnabledAnimationStateIterator();
                i.hasMoreElements();)
            {
                Ogre::AnimationState *anim = i.getNext();
                anim->addTime(dt);
            }
        }

        // play collision sounds
//        #ifndef DEBUG_NEWTON_BUILD
//        BOOST_FOREACH(colSnd &i, _colSnds)
//        {
//            using namespace engine::snd;
//            soundPtr s = _soundSys.play(i.name, true);
//            s->pos(i.pos);
//            s->radii(2, 5);
//            s->volume(i.vol);
//        }
//        #endif
//        _colSnds.clear();

        // if we are static and we have a velocity, manually move
        engine::vec3 v = vel();
        if (mass() < 0.01 && v.squaredLength() > 0.0001)
        {
            pos(pos() + v * dt);
            // since this is not normal movement, wake up the objects
            // which are in our aabb
            _world.wake(aabb());
        }
    }

    void genericProp::forceTorque(float dt)
    {
        engine::phys::body::forceTorque(dt);
    }

    void genericProp::aabbVisible(bool b)
    {
        if (_node)
            _node->showBoundingBox(b);
    }

    void genericProp::visible(bool b)
    {
        //active(b);
        _node->setVisible(b);
        _visible = b;
    }

    void genericProp::scale(const engine::vec3 &s)
    {
        if (s.x >= 0.01 && s.y >= 0.01 && s.z >= 0.01)
        {
            engine::phys::body::scale(s);
            _node->setScale(_origScale * s);
        }
    }

    void genericProp::clipPlane(const engine::vec4 &p)
    {
        if (_ent)
        {
            for (size_t i = 0; i < _ent->getNumSubEntities(); ++i)
            {
                Ogre::SubEntity *sub = _ent->getSubEntity(i);
                sub->setCustomParameter(engine::gfx::OBJECT_CLIP_PLANE_IDX, p);
            }
        }
        contactClipPlane = p;
    }

    void genericProp::load(const levelFormat::entity &desc)
    {
        entity::load(desc);

        using namespace engine;

        if (!desc.has_generic_prop_info())
        {
            log("generic prop " + _name + " has no generic_prop_info");
            return;
        }

        const levelFormat::genericPropInfo &info = desc.generic_prop_info();

        if (!info.has_script())
        {
            log("generic prop " + _name + " has no script");
            return;
        }

        // make sure we didn't load the script already
        if (info.script() != _docFileName)
            loadScript(info.script());

        // update default parameters
        if (info.has_position())
        {
            const levelFormat::vec3 &v = info.position();
            pos(vec3(v.x(), v.y(), v.z()));
        }

        if (info.has_rotation())
        {
            const levelFormat::quat &q = info.rotation();
            rot(quat(q.w(), q.x(), q.y(), q.z()));
        }

        if (info.has_scale())
        {
            const levelFormat::vec3 &s = info.scale();
            scale(vec3(s.x(), s.y(), s.z()));
        }

        if (info.has_mass())
            mass(info.mass());

        if (info.has_collidable())
            collidable(info.collidable());
    }

    void genericProp::save(levelFormat::entity &desc)
    {
        entity::save(desc);

        levelFormat::genericPropInfo *info = desc.mutable_generic_prop_info();

        info->set_script(_docFileName);

        using namespace engine;

        vec3 v = pos();
        levelFormat::vec3 *p = info->mutable_position();
        p->set_x(v.x);
        p->set_y(v.y);
        p->set_z(v.z);

        quat q = rot();
        levelFormat::quat *r = info->mutable_rotation();
        r->set_w(q.w);
        r->set_x(q.x);
        r->set_y(q.y);
        r->set_z(q.z);

        v = scale();
        levelFormat::vec3 *s = info->mutable_scale();
        s->set_x(v.x);
        s->set_y(v.y);
        s->set_z(v.z);

        info->set_mass(mass());
        info->set_collidable(collidable());
    }

    entityPtr genericProp::clone(const engine::string &name)
    {
        genericPropPtr p(new genericProp(name, *this));
        return p;
    }

    void genericProp::fromGenericProp(const genericProp &o)
    {
        // just in case we loaded something already
        _destroyEnt();

        _docFileName = o._docFileName;
        _origMaterial = o._origMaterial;
        _origScale = o._origScale;

        // same parent
        if (!_node)
            _node = o._node->getParentSceneNode()->createChildSceneNode();
        _node->setScale(o._node->getScale());

        if (o._ent)
        {
            _ent = _node->getCreator()->createEntity(_name + "_ent", o._ent->getMesh()->getName());
            for (unsigned i = 0; i < _ent->getNumSubEntities() && i < o._ent->getNumSubEntities(); ++i)
                _ent->getSubEntity(i)->setMaterialName(o._ent->getSubEntity(i)->getMaterialName());

            _node->attachObject(_ent);

            _ent->setVisibilityFlags(engine::gfx::MAIN_GROUP);//o._ent->getVisibilityFlags());

            // add animations, but for our own entity
            if (o._anims)
            {
                _anims = _ent->getAllAnimationStates();
                for (Ogre::ConstEnabledAnimationStateIterator i = o._anims->getEnabledAnimationStateIterator();
                    i.hasMoreElements();)
                {
                    Ogre::AnimationState *other = i.getNext();
                    Ogre::AnimationState *state = _anims->getAnimationState(other->getAnimationName());
                    state->setEnabled(true);
                    state->setWeight(1.0);
                    state->setLoop(other->getLoop());
                }
            }
            else
            {
                _anims = NULL;
            }

            for (size_t i = 0; i < _ent->getNumSubEntities(); ++i)
            {
                Ogre::SubEntity *sub = _ent->getSubEntity(i);
                sub->setCustomParameter(engine::gfx::OBJECT_CLIP_PLANE_IDX, engine::vec4(0, 0, 0, 1));
            }
        }

        matrix(o.matrix());
        // clone the collision to allow independent scaling
        collision(engine::phys::collisionPtr(o.collision()->clone()));

        collidable(o.collidable());
        swept(o.swept());
        autoSleep(o.autoSleep());

        scale(o.scale());
        // set the mass after the scale
        mass(o.mass());

        gravity = o.gravity;
    }

    void genericProp::useSkeletonFrom(const genericProp &gp)
    {
        assert(gp.ent() && _ent);
        if (gp.ent()->hasSkeleton() && _ent->hasSkeleton())
        {
            _ent->shareSkeletonInstanceWith(gp.ent());
            _anims = NULL;
        }
    }

    void genericProp::transform(const engine::vec3 &p, const engine::quat &q)
    {
        _node->setPosition(p);
        _node->setOrientation(q);
    }

    void genericProp::pos(const engine::vec3 &v)
    {
        engine::phys::body::pos(v);
        _node->setPosition(v);
    }

    void genericProp::rot(const engine::quat &q)
    {
        engine::phys::body::rot(q);
        _node->setOrientation(q);
    }

    void genericProp::matrix(const engine::mat4 &m)
    {
        engine::phys::body::matrix(m);
        _node->setPosition(m.getTrans());
        _node->setOrientation(m.extractQuaternion());
    }

    engine::mat4 genericProp::matrix() const
    {
        return engine::phys::body::matrix();
//        engine::mat4 m;
//        m.makeTransform(_node->_getDerivedPosition(), engine::vec3(1, 1, 1),
//            _node->_getDerivedOrientation());
//        return m;
    }

    //! interpolation is only graphical
    //! so return that position, not the actual body position
    engine::vec3 genericProp::pos() const
    {
        return engine::phys::body::pos();
//        return _node->_getDerivedPosition();
    }

    engine::quat genericProp::rot() const
    {
        return engine::phys::body::rot();
//        return _node->_getDerivedOrientation();
    }

    entityPtr genericPropFactory::create(const engine::string &name) const
    {
        using namespace engine;

        genericPropPtr ent(new genericProp(name, _params));

        return ent;
    }


}


