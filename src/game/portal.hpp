 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PORTAL_HPP_INCLUDED
#define PORTAL_HPP_INCLUDED

#include <list>

#include <map>

#include <boost/shared_ptr.hpp>
//#include <boost/function.hpp>
//#include <boost/signals/trackable.hpp>

#include <OgreMesh.h>

#include "pre_ogre.hpp"
#include "generic_prop.hpp"
#include "portal_joint.hpp"

namespace engine
{
    namespace gfx
    {
        class renderer;
    }
}

namespace game
{

    class portal;

    typedef boost::shared_ptr<portal> portalPtr;

    class portal: public genericProp//, public boost::signals::trackable
    {
        public:

            typedef std::vector<portal*> portalList;

        private:

            // connected portals hold a shared pointer to us
            // when we get removed from the world, we need to break
            // connections from them to us
            // (us to them is not necessarily the same in a circular connection, i.e.)
            std::list<portal*> _connectedToThis;

            struct copyInfo
            {
                genericPropPtr gp;
                portalJoint *joint;
            };

            // duplicates
            typedef std::map<genericProp*, copyInfo> copyMap;
            // first=original, second=copy
            copyMap _copies;

            // all portals (for rendering)
            static portalList _allPortals;

            portalPtr _connection;

            Ogre::Camera *_camera;

            Ogre::MeshPtr _partialProxy, _fullProxy;

            Ogre::Pass *_stencilPass, *_depthPass, *_clearDepthPass;

            friend class detector;
            class detector: public engine::phys::body
            {
                private:

//                    portal *_parent;
//
//                    //engine::phys::body *_objToTeleport;
//                    typedef std::list<genericProp*> touchList;
//                    touchList _touches;

                public:

                    typedef std::vector<genericProp*> touchList;
                    touchList touches;

                    detector(const engine::phys::world &w);

                    void processContacts(engine::phys::body &o,
                        const engine::phys::contactList &contacts,
                        engine::phys::contactList &rejections, float dt);
//
//                    //void contactsCriticalSection();
//                    void tick(float dt);
//
                    void forceTorque(float dt);
            };

            boost::shared_ptr<detector> _detector;

            // vel is seperate because it depends on our own velocity
            void _teleport(engine::phys::body &obj, const engine::vec3 &vel);

            void _render(Ogre::Camera *cam, Ogre::Viewport *vp, unsigned iter = 1);

            void _destroyCopy(copyMap::value_type &i);

        public:

            static const interfaceType TYPE;

            void renderSurface(const engine::vec3 &camPos) const;
            // render all portals
            static void renderAllPortals(Ogre::Camera *cam, Ogre::Viewport *vp, unsigned iter = 1);
            static const portalList &getAllPortals() { return _allPortals; }

            Ogre::Pass *stencilPass() const { return _stencilPass; }
            Ogre::Pass *depthPass() const { return _depthPass; }
            Ogre::Pass *clearDepthPass() const { return _clearDepthPass; }

            engine::vec4 plane() const;
            engine::vec3 normal() const;

            engine::vec3 relativeScale() const;

//            float radius() const;

            engine::mat4 matrixWithScale() const;

            void getRenderOp(const engine::vec3 &p, bool &depthCheck, Ogre::RenderOperation &rop) const;

            void scale(const engine::vec3 &s);
            engine::vec3 scale() const { return genericProp::scale(); }

            bool isBehindFace(const engine::vec3 &p) const;
            bool isWithinFace(const engine::vec3 &p) const;
            float distFromFace(const engine::vec3 &p) const;
            bool isVisible(Ogre::Camera *cam) const;
            bool isInside(const engine::vec3 &p) const;

            void willChangeTo(world *w);

            portal *connection()
            {
                if (_connection)
                    return _connection.get();
                return this;
            }
            const portal *connection() const
            {
                if (_connection)
                    return _connection.get();
                return this;
            }
            const engine::string &connectionName() const
            {
                return connection()->name();
            }
            void connection(const portalPtr &p);

            engine::vec3 teleportDir(const engine::vec3 &v);
            engine::vec3 teleportPos(const engine::vec3 &v);
            engine::mat4 teleportMatrix(const engine::mat4 &m);

            portal(
                const engine::string &name,
                const genericProp::constructParams &params);
            virtual ~portal();

            virtual void tick(engine::real dt);

            virtual void load(const levelFormat::entity &desc);
            virtual void save(levelFormat::entity &desc);

            virtual void reset();

            virtual void pos(const engine::vec3 &v);
            engine::vec3 pos() const;
            virtual void rot(const engine::quat &q);
            engine::quat rot() const;
            virtual void matrix(const engine::mat4 &m);
            engine::mat4 matrix() const;
    };

    class portalFactory: public entityFactory
    {
        private:

            genericProp::constructParams _params;

        public:

            portalFactory(const genericProp::constructParams &params):
                entityFactory(portal::TYPE),
                _params(params)
            {
            }

            entityPtr create(const engine::string &name) const;
    };

    typedef boost::shared_ptr<portalFactory> portalFactoryPtr;

}

#endif // PORTAL_HPP_INCLUDED
