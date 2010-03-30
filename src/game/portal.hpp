 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef PORTAL_HPP_INCLUDED
#define PORTAL_HPP_INCLUDED

#include <list>

#include <map>

#include <boost/shared_ptr.hpp>

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

    /// Shared pointer for the portal object
    typedef boost::shared_ptr<portal> portalPtr;

    /**
     * @class portal
     * @brief This class represents portal in game world
     */
    class portal: public genericProp//, public boost::signals::trackable
    {
        public:
			/// List of pointers for the portals
            typedef std::vector<portal*> portalList;

        private:

            // Connected portals hold a shared pointer to us
            // when we get removed from the world, we need to break
            // connections from them to us
            // (us to them is not necessarily the same in a circular connection, i.e.)

            /// This list consist of jois between portals
            std::list<portal*> _connectedToThis;

            struct copyInfo
            {
                genericPropPtr gp;
                portalJoint *joint;
            };

            // Duplicates
            typedef std::map<genericProp*, copyInfo> copyMap;
            // first=original, second=copy
            copyMap _copies;

            /// All portals (for rendering)
            static portalList _allPortals;

            /// Pointer to the portal to make connection with
            portalPtr _connection;

            /// Portal render camera. Such technology is used, for example
            /// in making mirrors in games.
            Ogre::Camera *_camera;

            Ogre::MeshPtr _partialProxy, _fullProxy;

            /// Passes for rendering portal
            Ogre::Pass *_stencilPass, *_depthPass, *_clearDepthPass;

            friend class detector;

            /**
             * @class detector
             * @brief Used for detecting activity with portal surface
             * @see class portal, class engine::phys::body
             */
            class detector: public engine::phys::body
            {
                private:

//                    portal *_parent;
//
//                    //engine::phys::body *_objToTeleport;
//                    typedef std::list<genericProp*> touchList;
//                    touchList _touches;

                public:
					// TODO: comment touchList generiProp
                    typedef std::vector<genericProp*> touchList;
                    touchList touches;

                    /**
                     * Constructor
                     * @param w physic world where detector will be used
                     */
                    detector(const engine::phys::world &w);

                    /**
                     * @brief Process contact between portals
                     */
                    void processContacts(engine::phys::body &o,
                        const engine::phys::contactList &contacts,
                        engine::phys::contactList &rejections, float dt);
//
//                    //void contactsCriticalSection();
//                    void tick(float dt);
//
                    /**
                     * @brief Force torque
                     * @param dt magnitude of torque
                     */
                    void forceTorque(float dt);
            };

            /// Shared pointer for the detecter sub-class
            boost::shared_ptr<detector> _detector;

            /**
             * @brief Teleport entity from touched portal into connected one
             * @param obj physic body to teleport
             * @param vel object velocity
             */
            void _teleport(engine::phys::body &obj, const engine::vec3 &vel);

            /**
             * @brief Render world from portal view
             */
            void _render(Ogre::Camera *cam, Ogre::Viewport *vp, unsigned iter = 1);

            void _destroyCopy(copyMap::value_type &i);

        public:

            static const interfaceType TYPE;

            /**
             * @brief Render portal surface
             * @param camPos from where to make surface
             */
            void renderSurface(const engine::vec3 &camPos) const;

            /**
             * @brief Render all portals
             */
            static void renderAllPortals(Ogre::Camera *cam, Ogre::Viewport *vp, unsigned iter = 1);

            /**
             * @brief Get all portals
             */
            static const portalList &getAllPortals() { return _allPortals; }

            /**
             * @brief Stencil pass
             */
            Ogre::Pass *stencilPass() const { return _stencilPass; }
            /**
             * @brief Depth pass
             */
            Ogre::Pass *depthPass() const { return _depthPass; }
            /**
             * @brief Clear depth pass
             */
            Ogre::Pass *clearDepthPass() const { return _clearDepthPass; }

            /// Portal surface plane
            engine::vec4 plane() const;
            /// Portal surface normal
            engine::vec3 normal() const;

            /// Used for scaling objects throught portals
            engine::vec3 relativeScale() const;

//            float radius() const;

            /// Used for scaling objects
            engine::mat4 matrixWithScale() const;

            /**
             * @brief Get portal render operation
             */
            void getRenderOp(const engine::vec3 &p, bool &depthCheck, Ogre::RenderOperation &rop) const;

            /**
             * @brief Scale object which is moved throught this portal
             * @param s scaling vector
             */
            void scale(const engine::vec3 &s);

            /**
             * @brief Get entity scaling
             */
            engine::vec3 scale() const { return genericProp::scale(); }

            // TODO: commen portal functions
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

    /**
     * Shared pointer to the current class
     */
    typedef boost::shared_ptr<portalFactory> portalFactoryPtr;
}

#endif // PORTAL_HPP_INCLUDED
