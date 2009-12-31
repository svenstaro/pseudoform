 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef SELECTION_NODE_HPP_INCLUDED
#define SELECTION_NODE_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include <OgreWireBoundingBox.h>

#include "pre_ogre.hpp"
#include "types.hpp"

namespace editor
{

    class selectionNode
    {
        public:

            enum gizmo_t
            {
                G_TRANSLATE, G_ROTATE, G_SCALE
            };

        private:

            Ogre::SceneNode *_aabbNode, *_node0, *_node1;

            Ogre::Entity *_gizmos[3][3]; // [type] [axis]
            Ogre::WireBoundingBox *_aabb;

            gizmo_t _currentGizmo;

            engine::vec3 _vec;

        public:

            engine::vec3 gizmoVec() const;
            engine::quat gizmoRot() const;
            engine::vec3 gizmoPos() const;

            bool active() const { return _vec.x > 0.01 || _vec.y > 0.01 || _vec.z > 0.01; }

            void setGizmoType(gizmo_t g);
            gizmo_t getGizmoType() const { return _currentGizmo; }
            void setGizmoVisible(size_t axis, bool b);

            void setVisible(bool b);

            void update(const Ogre::AxisAlignedBox &aabb, const engine::vec3 &pos, const engine::quat &rot);

            // keep the on-screen size constant based on camera distance
            void keepSize(const engine::vec3 &p);

            selectionNode(Ogre::SceneNode *parent);
            ~selectionNode();
    };

    typedef boost::shared_ptr<selectionNode> selectionNodePtr;

}

#endif // SELECTION_NODE_HPP_INCLUDED
