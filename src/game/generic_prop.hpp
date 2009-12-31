 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef GENERIC_PROP_HPP_INCLUDED
#define GENERIC_PROP_HPP_INCLUDED

#include <map>

#include "entity.hpp"
#include "phys/body.hpp"

#include "pre_ogre.hpp"
#include "types.hpp"

namespace engine
{
    namespace snd
    {
        class system;
    }
}

namespace game
{

    struct animInfo_t
    {
        engine::string name;
        bool loop;

        animInfo_t(const engine::string &n = "", bool b = true):
            name(n), loop(b) {}
    };

    typedef std::list<animInfo_t> animInfoList;

    class portal;

    class genericProp: public entity, public engine::phys::body
    {

        protected:

            Ogre::SceneNode *_node;
            Ogre::Entity *_ent;
            Ogre::AnimationStateSet *_anims;

            engine::string _docFileName;
            engine::string _origMaterial;

            engine::vec3 _origScale;

            bool _visible;

            engine::snd::system &_soundSys;

//            struct colSnd
//            {
//                engine::string name;
//                engine::vec3 pos;
//                float vol;
//            };
//
//            typedef std::list<colSnd> colSndList;
//            colSndList _colSnds;

            void _destroyEnt();
            void _destroyNode();

        public:

            portal *touchingPortal;

            struct constructParams
            {
                Ogre::SceneNode *parentNode;
                const engine::phys::world &world;
                engine::snd::system &soundSys;
                bool inGame;

                constructParams(Ogre::SceneNode *parentNode,
                    const engine::phys::world &world,
                    engine::snd::system &soundSys,
                    bool inGame):
                    parentNode(parentNode),
                    world(world),
                    soundSys(soundSys),
                    inGame(inGame)
                {
                }
            };

            static const interfaceType TYPE;

            const engine::string &origMaterial() const { return _origMaterial; }
            void setMaterial(const engine::string &str);

            virtual Ogre::SceneNode *node() const { return _node; }
            virtual Ogre::Entity *ent() const { return _ent; }

            genericProp(const engine::string &n, const constructParams &params);
            genericProp(const engine::string &n, const genericProp &o);
            virtual ~genericProp();

            void useSkeletonFrom(const genericProp &gp);

            // utility to auto-load from another of us
            void fromGenericProp(const genericProp &o);

            virtual void reset(); // reset and clear history

            // load a script to create us
            void loadScript(const engine::string &fn);
            const engine::string getScript() const { return _docFileName; }

            virtual void tick(engine::real dt);

            // callback from physics
            virtual void transform(const engine::vec3 &p, const engine::quat &q);

            // sync node with physics
            virtual void pos(const engine::vec3 &v);
            engine::vec3 pos() const;
            virtual void rot(const engine::quat &q);
            engine::quat rot() const;
            virtual void matrix(const engine::mat4 &m);
            engine::mat4 matrix() const;

            virtual void aabbVisible(bool b);
            virtual void visible(bool b);
            virtual void scale(const engine::vec3 &s);
            virtual engine::vec3 scale() const { return engine::phys::body::scale(); }
            virtual void clipPlane(const engine::vec4 &p);

            virtual void load(const levelFormat::entity &desc);
            virtual void save(levelFormat::entity &desc);

            virtual void forceTorque(float dt);

            virtual void processContacts(engine::phys::body &o,
                const engine::phys::contactList &contacts,
                engine::phys::contactList &rejections, float dt);

            virtual entityPtr clone(const engine::string &name);

    };

    typedef boost::shared_ptr<genericProp> genericPropPtr;

    class genericPropFactory: public entityFactory
    {
        private:

            genericProp::constructParams _params;

        public:

            genericPropFactory(const genericProp::constructParams &params):
                entityFactory(genericProp::TYPE),
                _params(params)
            {
            }

            entityPtr create(const engine::string &name) const;
    };

    typedef boost::shared_ptr<genericPropFactory> genericPropFactoryPtr;

}

#endif // GENERIC_PROP_HPP_INCLUDED
