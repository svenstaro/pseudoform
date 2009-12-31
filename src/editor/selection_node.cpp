 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreAxisAlignedBox.h>

#include <cmath>

#include "selection_node.hpp"

#include "gfx/renderer.hpp"

namespace editor
{

    selectionNode::selectionNode(Ogre::SceneNode *parent):
        _aabbNode(NULL),
        _node0(NULL),
        _node1(NULL),
        _currentGizmo(G_TRANSLATE),
        _vec(0, 0, 0)
    {
        _aabbNode = parent->createChildSceneNode();
        _node0 = parent->createChildSceneNode();
        _node1 = _node0->createChildSceneNode();

        _aabb = new Ogre::WireBoundingBox();
        _aabb->setMaterial("aabb_template");
        _aabb->setVisible(false);
        _aabb->setVisibilityFlags(engine::gfx::OVERLAY_GROUP);
        _aabbNode->attachObject(_aabb);

        Ogre::SceneManager *sceneMgr = parent->getCreator();

        _gizmos[G_TRANSLATE][0] = sceneMgr->createEntity("selectionNode::_gizmos[0][0]", "editor/gizmo_move_x.mesh");
        _gizmos[G_TRANSLATE][0]->setMaterialName("red");
        _gizmos[G_TRANSLATE][1] = sceneMgr->createEntity("selectionNode::_gizmos[0][1]", "editor/gizmo_move_y.mesh");
        _gizmos[G_TRANSLATE][1]->setMaterialName("green");
        _gizmos[G_TRANSLATE][2] = sceneMgr->createEntity("selectionNode::_gizmos[0][2]", "editor/gizmo_move_z.mesh");
        _gizmos[G_TRANSLATE][2]->setMaterialName("blue");

        _gizmos[G_ROTATE][0] = sceneMgr->createEntity("selectionNode::_gizmos[1][0]", "editor/gizmo_rotate_x.mesh");
        _gizmos[G_ROTATE][0]->setMaterialName("red");
        _gizmos[G_ROTATE][1] = sceneMgr->createEntity("selectionNode::_gizmos[1][1]", "editor/gizmo_rotate_y.mesh");
        _gizmos[G_ROTATE][1]->setMaterialName("green");
        _gizmos[G_ROTATE][2] = sceneMgr->createEntity("selectionNode::_gizmos[1][2]", "editor/gizmo_rotate_z.mesh");
        _gizmos[G_ROTATE][2]->setMaterialName("blue");

        _gizmos[G_SCALE][0] = sceneMgr->createEntity("selectionNode::_gizmos[2][0]", "editor/gizmo_compass_x.mesh");
        _gizmos[G_SCALE][0]->setMaterialName("red");
        _gizmos[G_SCALE][1] = sceneMgr->createEntity("selectionNode::_gizmos[2][1]", "editor/gizmo_compass_y.mesh");
        _gizmos[G_SCALE][1]->setMaterialName("green");
        _gizmos[G_SCALE][2] = sceneMgr->createEntity("selectionNode::_gizmos[2][2]", "editor/gizmo_compass_z.mesh");
        _gizmos[G_SCALE][2]->setMaterialName("blue");

        // translation is AABB-relative, scaling is local, rotating is local
        _node0->attachObject(_gizmos[G_TRANSLATE][0]);
        _node0->attachObject(_gizmos[G_TRANSLATE][1]);
        _node0->attachObject(_gizmos[G_TRANSLATE][2]);

        _node0->attachObject(_gizmos[G_ROTATE][0]);
        _node0->attachObject(_gizmos[G_ROTATE][1]);
        _node0->attachObject(_gizmos[G_ROTATE][2]);

        _node1->attachObject(_gizmos[G_SCALE][0]);
        _node1->attachObject(_gizmos[G_SCALE][1]);
        _node1->attachObject(_gizmos[G_SCALE][2]);

        for (size_t i = 0; i < 3; ++i)
            for (size_t j = 0; j < 3; j++)
            {
                _gizmos[i][j]->setVisible(false);
                _gizmos[i][j]->setVisibilityFlags(engine::gfx::OVERLAY_GROUP);
            }
    }

    engine::vec3 selectionNode::gizmoVec() const
    {
        //return _gizmoNode->_getDerivedOrientation() * _vec;
        return _vec;
    }

    engine::quat selectionNode::gizmoRot() const
    {
        return _node1->_getDerivedOrientation();
    }

    selectionNode::~selectionNode()
    {
        _aabbNode->getParentSceneNode()->removeAndDestroyChild(_aabbNode->getName());
        _node0->getParentSceneNode()->removeAndDestroyChild(_node0->getName());
        _aabbNode = NULL;
        _node0 = NULL;
        _node1 = NULL;

        delete _aabb;

        for (size_t i = 0; i < 3; ++i)
            for (size_t j = 0; j < 3; j++)
                _gizmos[i][j]->_getManager()->destroyEntity(_gizmos[i][j]);
    }

    void selectionNode::setGizmoType(selectionNode::gizmo_t g)
    {
        assert(g < 3 && "only 3 gizmos, but g >= 3");

        // reset visibility
        for (size_t i = 0; i < 3; ++i)
            for (size_t j = 0; j < 3; j++)
                _gizmos[i][j]->setVisible(false);

        _currentGizmo = g;
    }

    void selectionNode::setGizmoVisible(size_t axis, bool b)
    {
        if (!_aabb->getVisible() && b) // if no bounding box, then no gizmos
            return;

        assert(axis < 3 && "axis should be 0-2");

        // set visibility
        _gizmos[_currentGizmo][axis]->setVisible(b);

        // convert into a vector
        _vec[axis] = float(int(b));
    }

    void selectionNode::setVisible(bool b)
    {
        //_aabbNode->setVisible(b);
        _aabb->setVisible(b);
    }

    void selectionNode::update(const Ogre::AxisAlignedBox &aabb, const engine::vec3 &pos, const engine::quat &rot)
    {
        _node0->setPosition(pos);//aabb.getCenter());
        _node1->setOrientation(rot);
        _aabb->setupBoundingBox(aabb);

        // for some odd reason, without the below line, the AABB
        // won't appear =/
        _aabbNode->setPosition(0, 0, 0);
//        _aabb->setVisible(true);
//        _aabbNode->setVisible(true);
    }

    engine::vec3 selectionNode::gizmoPos() const
    {
        return _node0->_getDerivedPosition();
    }

    void selectionNode::keepSize(const engine::vec3 &p)
    {
        _node0->setScale(engine::vec3(1, 1, 1) * std::sqrt((p - _node0->_getDerivedPosition()).length()));
    }

}
