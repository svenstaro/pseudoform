 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
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

/**
 * @namespace game
 * @brief Consist of tools for building game world
 */
namespace game
{
	/// Consists of information about animation
    struct animInfo_t
    {
    	/// Name of need animation state
        engine::string name;

        /// Loop animation?
        bool loop;

        /**
         * Constructor
         */
        animInfo_t(const engine::string &n = "", bool b = true):
            name(n), loop(b) {}
    };

    /// List of animation states
    typedef std::list<animInfo_t> animInfoList;

    class portal;

    /**
     * @class genericProp
     * @brief Represents object in world based on entity and physic object
     * @see class entity, class engine::phys::body
     */
    class genericProp: public entity, public engine::phys::body
    {
        protected:
			/// Node to attach object to
            Ogre::SceneNode *_node;
            /// Entity to attach for node
            Ogre::Entity *_ent;
            /// List of animations
            Ogre::AnimationStateSet *_anims;

            // Name of the script applyed for the object
            engine::string _docFileName;

            /// Original material TODO ?
            engine::string _origMaterial;

            /// Default scale
            engine::vec3 _origScale;

            /// Render it?
            bool _visible;

            /// Sounds that refer to this body
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

            /**
             * @brief Remove current object entity
             */
            void _destroyEnt();
            /**
             * @brief Remove current object node
             */
            void _destroyNode();

        public:
            // TODO: Wtf? Touching portal
            portal *touchingPortal;

            // TODO: ? constructParams
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

            /**
             * @brief Get object original material
             * @return string of original material
             */
            const engine::string &origMaterial() const { return _origMaterial; }

            /**
             * @brief Set material
             * @param str new material name
             */
            void setMaterial(const engine::string &str);

            /**
             * @brief Get scene node
             */
            virtual Ogre::SceneNode *node() const { return _node; }
            /**
             * @brief Get entity
             */
            virtual Ogre::Entity *ent() const { return _ent; }

            /**
             * @brief Constructor #1
             * @param n name of the prop
             * @param params params to apply for the prop
             */
            genericProp(const engine::string &n, const constructParams &params);
            /**
             * @brief Constructor #2
             * @param n new name of prop
             * @param o prop to copy
             */
            genericProp(const engine::string &n, const genericProp &o);

            /**
             * Destructor
             */
            virtual ~genericProp();

            /**
             * @brief Use skeleton from given prop
             */
            void useSkeletonFrom(const genericProp &gp);

            /**
             * @brief Utility to auto-load from another of us
             */
            void fromGenericProp(const genericProp &o);

            /**
             * @brief Reset and clear history
             */
            virtual void reset();

            /**
             * @brief Load a script to create us
             * @param fn script name
             */
            void loadScript(const engine::string &fn);
            /**
             * @brief Get script for current object
             */
            const engine::string getScript() const { return _docFileName; }

            /**
             * @brief Used for global application cycle (updating)
             */
            virtual void tick(engine::real dt);

            /**
             * @brief Callback from physics
             */
            virtual void transform(const engine::vec3 &p, const engine::quat &q);

            // sync node with physics
            /**
             * @brief Synchronize object's node with physics
             * @param v new vector position for the node
             */
            virtual void pos(const engine::vec3 &v);
            /**
             * @brief Get position of the node
             */
            engine::vec3 pos() const;

            /**
             * @brief Synchronize rotation of the node with physics
             * @param q quaternion that represents new rotation to apply
             */
            virtual void rot(const engine::quat &q);
            /**
             * @brief Get node rotation
             */
            engine::quat rot() const;

            /**
             * @brief Set up object matrix
             * @param m matrix to apply
             */
            virtual void matrix(const engine::mat4 &m);
            /**
             * @brief Get object matrix
             */
            engine::mat4 matrix() const;

            // TODO: Wtf aabbVisible
            virtual void aabbVisible(bool b);

            /**
             * @brief Set object visibility
             */
            virtual void visible(bool b);

            /**
             * @brief Scale object
             * @param s scaling vector
             */
            virtual void scale(const engine::vec3 &s);

			/**
			 * @brief Get object physics dimensions
			 */
            virtual engine::vec3 scale() const { return engine::phys::body::scale(); }

            /**
             * @brief Clip plane
             * @param p vector 4 that represents clipping plane
             */
            virtual void clipPlane(const engine::vec4 &p);

            /**
             * @brief Load entity
             */
            virtual void load(const levelFormat::entity &desc);
            /**
             * @brief Save entity
             */
            virtual void save(levelFormat::entity &desc);

            /**
             * @brief Apply force to the body
             * @param dt TODO: param dt forceTorque
             */
            virtual void forceTorque(float dt);

            /**
             * @brief Process contacts between 2 physics bodies
             * @remarks see newton game dynamice: physics materials
             */
            virtual void processContacts(engine::phys::body &o,
                const engine::phys::contactList &contacts,
                engine::phys::contactList &rejections, float dt);

            /**
             * @brief Clone entity
             * @param name entity to clone
             * @return shared pointer to the cloned version of entity
             */
            virtual entityPtr clone(const engine::string &name);

    };

    /// Shared pointer for the prop
    typedef boost::shared_ptr<genericProp> genericPropPtr;

    /**
     * @class genericPropFactory
     * @brief Prop factory
     * @see class genericProp
     * @remarks for more information
     * visit: http://en.wikipedia.org/wiki/Factory_method_pattern
     */
    class genericPropFactory: public entityFactory
    {
        private:
            genericProp::constructParams _params;

        public:
			/**
			 * Constructor
			 */
            genericPropFactory(const genericProp::constructParams &params):
                entityFactory(genericProp::TYPE),
                _params(params)
            {
            }

            /**
             * @brief Create new entity for the factory
             * @param name new entity name
             */
            entityPtr create(const engine::string &name) const;
    };

    /**
     * Shared pointer to the current class
     */
    typedef boost::shared_ptr<genericPropFactory> genericPropFactoryPtr;
}

#endif // GENERIC_PROP_HPP_INCLUDED
