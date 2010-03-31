 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SELECTION_NODE_HPP_INCLUDED
#define SELECTION_NODE_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include <OgreWireBoundingBox.h>

#include "pre_ogre.hpp"
#include "types.hpp"

/**
 * @namespace editor
 * @brief Consist of tools for working with objects in editor mode
 */
namespace editor
{
	/**
	 * @class selectionNode
	 * @brief Represents doings for selected node in editor state
	 */
    class selectionNode
    {
        public:
			/// Available doings
            enum gizmo_t
            {
                G_TRANSLATE, G_ROTATE, G_SCALE
            };

        private:
            /// Additional nodes for showing up doings with selected node
            Ogre::SceneNode *_aabbNode, *_node0, *_node1;

            /// Consist of different axises for different types
            Ogre::Entity *_gizmos[3][3]; // [type] [axis]

            /// Bound box for selected node
            Ogre::WireBoundingBox *_aabb;

            /// Chosen gizmo to use at current time
            gizmo_t _currentGizmo;

            /// Vector to apply gizmo for
            engine::vec3 _vec;

        public:
            /**
             * @brief Get vizmo vector
             */
            engine::vec3 gizmoVec() const;

            /**
             * @brief Get gizmo rotation
             */
            engine::quat gizmoRot() const;

            /**
             * @brief Get gizmo position
             */
            engine::vec3 gizmoPos() const;

            bool active() const { return _vec.x > 0.01 || _vec.y > 0.01 || _vec.z > 0.01; }

            /**
             * @brief Set doing-gizmo type
             */
            void setGizmoType(gizmo_t g);

            /**
             * @brief Get active gizmo type
             */
            gizmo_t getGizmoType() const { return _currentGizmo; }

            /**
             * @brief Set gizmo's axis visible
             */
            void setGizmoVisible(size_t axis, bool b);

            /**
             * @brief Set visible all gizmo's axises
             */
            void setVisible(bool b);

            /**
             * @brief Update gizmo
             */
            void update(const Ogre::AxisAlignedBox &aabb, const engine::vec3 &pos, const engine::quat &rot);

            /*
             * @brief Keep the on-screen size constant based on camera distance
             */
            void keepSize(const engine::vec3 &p);

            /**
             * Constructor
             */
            selectionNode(Ogre::SceneNode *parent);

            /**
             * Destructor
             */
            ~selectionNode();
    };

    /**
     * Shared pointer to the current class
     */
    typedef boost::shared_ptr<selectionNode> selectionNodePtr;
}

#endif // SELECTION_NODE_HPP_INCLUDED
