 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <Newton.h>

#include <OgreMesh.h>
#include <OgreSubMesh.h>
#include <OgreMeshManager.h>

#include "types.hpp"
#include "collision.hpp"
#include "log.hpp"
#include "world.hpp"

#include "error_mesh.hpp"

namespace engine
{
    namespace phys
    {
        collision::~collision()
        {
            NewtonReleaseCollision(_world, _col);
            _col = NULL;
        }

        nullCollision::nullCollision(const world &w):
            collision(w)
        {
            _col = NewtonCreateNull(w);
        }

        // assumes ownership of 'c' and its reference count
        primitiveCollision::primitiveCollision(const world &w, NewtonCollision *c):
            collision(w),
            _orig(NULL)
        {
            // store original for cloning
            _orig = c;
            // create a modifier for any primitive so we can scale
            _col = NewtonCreateConvexHullModifier(w, _orig, 0);
//            _col = _orig;
//            NewtonAddCollisionReference(_col);
        }

        primitiveCollision::~primitiveCollision()
        {
            // destroy original, collision::~collision() will destroy _col
            NewtonReleaseCollision(_world, _orig);
        }

        primitiveCollision *primitiveCollision::clone() const
        {
            // just add another ownership
            NewtonAddCollisionReference(_orig);
            return new primitiveCollision(_world, _orig);
        }

        void primitiveCollision::scale(const vec3 &v)
        {
            _scale = v;

            float mat[16];
            NewtonConvexHullModifierSetMatrix(_col, _matrix(mat4::getScale(_scale), mat));
        }

        float primitiveCollision::volume() const
        {
            return NewtonConvexCollisionCalculateVolume(_col);
        }

        convexHullCollision::convexHullCollision(const world &w, const string &meshName, const vec3 &s):
            collision(w),
            _orig(NULL)
        {
            Ogre::MeshPtr meshPtr;
            try
            {
                meshPtr = Ogre::MeshPtr(Ogre::MeshManager::getSingleton().load(meshName,
                    Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
            }
            catch(...)
            {
                log("mesh \"" + meshName + "\" invalid for collision");
            }

            if (!meshPtr.get())
            {
                try
                {
                    meshPtr = Ogre::MeshManager::getSingleton().load(
                        ERROR_MESH, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
                }
                catch(...)
                {
                    log("missing " + ERROR_MESH);
                    // simple collision when something fails
                    NewtonCollision *col = NewtonCreateSphere(w, 0.5, 0.5, 0.5, 0, NULL);
                    _col = NewtonCreateConvexHullModifier(w, col, 0);
                    // release original
                    NewtonReleaseCollision(w, col);
                    return;
                }
            }

            const Ogre::Mesh &mesh = *meshPtr;

            std::vector<vec3> verts;
            for (unsigned smi = 0; smi < mesh.getNumSubMeshes(); smi++) {
                Ogre::SubMesh *subMesh = mesh.getSubMesh(smi);

                //log("adding submesh " + boost::lexical_cast<string>(smi));

                Ogre::VertexData *vertexData =
                    (subMesh->useSharedVertices) ?
                        vertexData = mesh.sharedVertexData : vertexData = subMesh->vertexData;

                Ogre::VertexDeclaration *vertexDecl = vertexData->vertexDeclaration;
                const Ogre::VertexElement *element = vertexDecl->findElementBySemantic(Ogre::VES_POSITION);

                Ogre::HardwareVertexBufferSharedPtr vertexHVBSP =
                    vertexData->vertexBufferBinding->getBuffer(element->getSource());
                unsigned char *vPtr = (unsigned char*)(vertexHVBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

                Ogre::IndexData *indexData = subMesh->indexData;
                size_t numIndices = indexData->indexCount;
                size_t numTris = numIndices / 3;

                // get pointer!
                Ogre::HardwareIndexBufferSharedPtr indexHIBSP = indexData->indexBuffer;

                // 16 or 32 bit indices?
                bool indicesAre32Bit = (indexHIBSP->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
                unsigned long *longPtr = NULL;
                unsigned short *shortPtr = NULL;

                if (indicesAre32Bit)
                    longPtr =
                        static_cast<unsigned long*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
                else
                    shortPtr =
                        static_cast<unsigned short*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

                //now loop through the indices, getting polygon info!
                int iOffset = 0;

                for (size_t i = 0; i < numTris; i++) {
                    unsigned char *vOffset;
                    float *vertexPosPtr;
                    int idx;

                    for (int j = 0; j < 3; j++) {
                        if (indicesAre32Bit)
                            idx = longPtr[iOffset + j];
                        else
                            idx = shortPtr[iOffset + j];

                        vOffset = vPtr + (idx * vertexHVBSP->getVertexSize());
                        element->baseVertexPointerToElement(vOffset, &vertexPosPtr);

                        vec3 vert;

                        vert.x = *vertexPosPtr; vertexPosPtr++;
                        vert.y = *vertexPosPtr; vertexPosPtr++;
                        vert.z = *vertexPosPtr; vertexPosPtr++;

                        // 's' is a preliminary scale
                        verts.push_back(vert * s);
                    }

                    iOffset += 3;
                }

                //unlock the buffers!
                vertexHVBSP->unlock();
                indexHIBSP->unlock();
            }

            // store original
            _orig = NewtonCreateConvexHull(w, verts.size(), &verts[0].x, 12, 0.01, 0, NULL);
            _col = NewtonCreateConvexHullModifier(w, _orig, 0);
        }

        // construct from another already-constructed convex collision
        // it already added an extra reference count, so we can 'own' this
        convexHullCollision::convexHullCollision(const world &w, NewtonCollision *orig):
            collision(w),
            _orig(orig)
        {
            _col = NewtonCreateConvexHullModifier(w, _orig, 0);
        }

        convexHullCollision::~convexHullCollision()
        {
            // collision::~collision will get rid of _col, but not _orig
            NewtonReleaseCollision(_world, _orig);
        }

        convexHullCollision *convexHullCollision::clone() const
        {
            // just add another ownership
            NewtonAddCollisionReference(_orig);
            return new convexHullCollision(_world, _orig);
        }

        void convexHullCollision::scale(const vec3 &v)
        {
            _scale = v;

            float mat[16];
            NewtonConvexHullModifierSetMatrix(_col, _matrix(mat4::getScale(_scale), mat));
        }

        float convexHullCollision::volume() const
        {
            return NewtonConvexCollisionCalculateVolume(_col);
        }

        concaveHullCollision::concaveHullCollision(const world &w, const string &meshName, const vec3 &s):
            collision(w)
        {
            Ogre::MeshPtr meshPtr;
            try
            {
                meshPtr = Ogre::MeshPtr(Ogre::MeshManager::getSingleton().load(meshName,
                    Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
            }
            catch(...)
            {
                log("mesh \"" + meshName + "\" invalid for collision");
            }

            if (!meshPtr.get())
            {
                try
                {
                    meshPtr = Ogre::MeshManager::getSingleton().load(
                        ERROR_MESH, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
                }
                catch(...)
                {
                    log("missing " + ERROR_MESH);
                    // simple collision when something fails
                    NewtonCollision *col = NewtonCreateSphere(w, 0.5, 0.5, 0.5, 0, NULL);
                    _col = NewtonCreateConvexHullModifier(w, col, 0);
                    // release original
                    NewtonReleaseCollision(w, col);
                    return;
                }
            }

            const Ogre::Mesh &mesh = *meshPtr;

            for (unsigned smi = 0; smi < mesh.getNumSubMeshes(); smi++) {
                std::vector<vec3> partVerts;

                Ogre::SubMesh *subMesh = mesh.getSubMesh(smi);

                Ogre::VertexData *vertexData =
                    (subMesh->useSharedVertices) ?
                        vertexData = mesh.sharedVertexData : vertexData = subMesh->vertexData;

                Ogre::VertexDeclaration *vertexDecl = vertexData->vertexDeclaration;
                const Ogre::VertexElement *element = vertexDecl->findElementBySemantic(Ogre::VES_POSITION);

                Ogre::HardwareVertexBufferSharedPtr vertexHVBSP =
                    vertexData->vertexBufferBinding->getBuffer(element->getSource());
                unsigned char *vPtr = (unsigned char*)(vertexHVBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

                Ogre::IndexData *indexData = subMesh->indexData;
                size_t numIndices = indexData->indexCount;
                size_t numTris = numIndices / 3;

                // get pointer!
                Ogre::HardwareIndexBufferSharedPtr indexHIBSP = indexData->indexBuffer;

                // 16 or 32 bit indices?
                bool indicesAre32Bit = (indexHIBSP->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
                unsigned long *longPtr = NULL;
                unsigned short *shortPtr = NULL;

                if (indicesAre32Bit)
                    longPtr =
                        static_cast<unsigned long*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
                else
                    shortPtr =
                        static_cast<unsigned short*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

                //now loop through the indices, getting polygon info!
                int iOffset = 0;

                for (size_t i = 0; i < numTris; i++) {
                    unsigned char *vOffset;
                    float *vertexPosPtr;
                    int idx;

                    for (int j = 0; j < 3; j++) {
                        if (indicesAre32Bit)
                            idx = longPtr[iOffset + j];
                        else
                            idx = shortPtr[iOffset + j];

                        vOffset = vPtr + (idx * vertexHVBSP->getVertexSize());
                        element->baseVertexPointerToElement(vOffset, &vertexPosPtr);

                        vec3 vert;

                        vert.x = *vertexPosPtr; vertexPosPtr++;
                        vert.y = *vertexPosPtr; vertexPosPtr++;
                        vert.z = *vertexPosPtr; vertexPosPtr++;

                        partVerts.push_back(vert * s);
                    }

                    iOffset += 3;
                }

                //unlock the buffers!
                vertexHVBSP->unlock();
                indexHIBSP->unlock();

                // create a convex hull for this sub mesh
                NewtonCollision *col = NewtonCreateConvexHull(w, partVerts.size(), &partVerts[0].x, 12, 0.01, 0, NULL);
                // add original to list of originals
                _origs.push_back(col);
                // create a modifier for this hull
                NewtonCollision *mod = NewtonCreateConvexHullModifier(w, col, 0);
                // add modifier to parts list
                _parts.push_back(mod);
            }

            // create compound out of all the convex hull modifiers
            _col = NewtonCreateCompoundCollision(w, _parts.size(), &_parts[0], 0);
        }

        concaveHullCollision::concaveHullCollision(const world &w, const std::vector<NewtonCollision*> &origs):
            collision(w)
        {
            _origs = origs;
            // create modifiers
            for (size_t i = 0; i < _origs.size(); ++i)
                _parts.push_back(NewtonCreateConvexHullModifier(w, _origs[i], 0));
            // create compound out of all the convex hull modifiers
            _col = NewtonCreateCompoundCollision(w, _parts.size(), &_parts[0], 0);
        }

        concaveHullCollision::~concaveHullCollision()
        {
            // collision::~collision will get rid of _col, but not _origs or _parts
            for (size_t i = 0; i < _origs.size(); ++i)
                NewtonReleaseCollision(_world, _origs[i]);
            for (size_t i = 0; i < _parts.size(); ++i)
                NewtonReleaseCollision(_world, _parts[i]);
        }

        void concaveHullCollision::scale(const vec3 &v)
        {
            _scale = v;

            float mat[16];
            // save the matrix
            _matrix(mat4::getScale(_scale), mat);

            // to scale a concave collision, we need to scale each individual
            // part of the compound collision.  since they are all modifiers,
            // this is very simple
            for (size_t i = 0; i < _parts.size(); ++i)
                NewtonConvexHullModifierSetMatrix(_parts[i], mat);
        }

        float concaveHullCollision::volume() const
        {
            float vol = 0;
            // for a concave hull, we need to add up the volume
            // of each and every part
            for (size_t i = 0; i < _parts.size(); ++i)
                vol += NewtonConvexCollisionCalculateVolume(_parts[i]);
            return vol;
        }

        concaveHullCollision *concaveHullCollision::clone() const
        {
            // add another reference count to all of the originals
            for (size_t i = 0; i < _origs.size(); ++i)
                NewtonAddCollisionReference(_origs[i]);
            // and create a concave hull from this
            return new concaveHullCollision(_world, _origs);
        }

        treeCollision::treeCollision(const world &w, const string &meshName, const vec3 &s):
            collision(w),
            _meshName(meshName),
            _origScale(s)
        {
            create(s);
        }

        void treeCollision::scale(const vec3 &v)
        {
            _scale = v;

            // to scale a tree collision, we need to actually reload the model
            // and reparse it :| not too fast, but it's the only way

            // destroy original
            NewtonReleaseCollision(_world, _col);
            _col = NULL;

            // recreate
            create(_scale);
        }

        treeCollision *treeCollision::clone() const
        {
            // just reparse the mesh, nothing better to do
            return new treeCollision(_world, _meshName, _origScale);
        }

        void treeCollision::create(const vec3 &s)
        {
            Ogre::MeshPtr meshPtr;
            try
            {
                meshPtr = Ogre::MeshPtr(Ogre::MeshManager::getSingleton().load(_meshName,
                    Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
            }
            catch(...)
            {
                log("mesh \"" + _meshName + "\" invalid for collision");
            }

            if (!meshPtr.get())
            {
                try
                {
                    meshPtr = Ogre::MeshManager::getSingleton().load(
                        ERROR_MESH, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
                }
                catch(...)
                {
                    log("missing " + ERROR_MESH);
                    // simple collision when something fails
                    NewtonCollision *col = NewtonCreateSphere(_world, 0.5, 0.5, 0.5, 0, NULL);
                    _col = NewtonCreateConvexHullModifier(_world, col, 0);
                    // release original
                    NewtonReleaseCollision(_world, col);
                    return;
                }
            }

            const Ogre::Mesh &mesh = *meshPtr;

            _col = NewtonCreateTreeCollision(_world, 0);
            NewtonTreeCollisionBeginBuild(_col);

            // if our tree, use min of int.  otherwise, use passed in id
            int id = std::numeric_limits<int>::min();

            for (unsigned smi = 0; smi < mesh.getNumSubMeshes(); smi++) {
                Ogre::SubMesh *subMesh = mesh.getSubMesh(smi);

                Ogre::VertexData *vertexData =
                    (subMesh->useSharedVertices) ?
                        vertexData = mesh.sharedVertexData : vertexData = subMesh->vertexData;

                Ogre::VertexDeclaration *vertexDecl = vertexData->vertexDeclaration;
                const Ogre::VertexElement *element = vertexDecl->findElementBySemantic(Ogre::VES_POSITION);

                Ogre::HardwareVertexBufferSharedPtr vertexHVBSP =
                    vertexData->vertexBufferBinding->getBuffer(element->getSource());
                unsigned char *vPtr = (unsigned char*)(vertexHVBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

                Ogre::IndexData *indexData = subMesh->indexData;
                size_t numIndices = indexData->indexCount;
                size_t numTris = numIndices / 3;

                // get pointer!
                Ogre::HardwareIndexBufferSharedPtr indexHIBSP = indexData->indexBuffer;

                // 16 or 32 bit indices?
                bool indicesAre32Bit = (indexHIBSP->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
                unsigned long *longPtr = NULL;
                unsigned short *shortPtr = NULL;

                if (indicesAre32Bit)
                    longPtr =
                        static_cast<unsigned long*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
                else
                    shortPtr =
                        static_cast<unsigned short*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

                //now loop through the indices, getting polygon info!
                int iOffset = 0;

                for (size_t i = 0; i < numTris; i++) {
                    vec3 triVertices[3];
                    unsigned char *vOffset;
                    float *vertexPosPtr;
                    int idx;

                    for (int j = 0; j < 3; j++) {
                        if (indicesAre32Bit)
                            idx = longPtr[iOffset + j];
                        else
                            idx = shortPtr[iOffset + j];

                        vOffset = vPtr + (idx * vertexHVBSP->getVertexSize());
                        element->baseVertexPointerToElement(vOffset, &vertexPosPtr);

                        triVertices[j].x = *vertexPosPtr; vertexPosPtr++;
                        triVertices[j].y = *vertexPosPtr; vertexPosPtr++;
                        triVertices[j].z = *vertexPosPtr; vertexPosPtr++;

                        triVertices[j] = triVertices[j] * s;
                    }

                    NewtonTreeCollisionAddFace(_col, 3, &triVertices[0].x, sizeof(float) * 3, id++);

                    iOffset += 3;
                }

                //unlock the buffers!
                vertexHVBSP->unlock();
                indexHIBSP->unlock();
            }

            NewtonTreeCollisionEndBuild(_col, 1);
        }

        float treeCollision::volume() const
        {
            // tree collision can't calculate volume
            return 1;
        }

        collisionPtr box(const world &w, const vec3 &diameter)
        {
            return collisionPtr(new primitiveCollision(
                w, NewtonCreateBox(w, diameter.x, diameter.y, diameter.z, 0, NULL)));
        }

        collisionPtr ball(const world &w, const vec3 &diameter)
        {
            return collisionPtr(new primitiveCollision(
                w, NewtonCreateSphere(w, diameter.x * 0.5, diameter.y * 0.5, diameter.z * 0.5, 0, NULL)));
        }

        collisionPtr cone(const world &w, float diameter, float height)
        {
            return collisionPtr();
        }

        collisionPtr cylinder(const world &w, float diameter, float height)
        {
            const quat ROT90Z(deg(-90), vec3(0, 0, 1));
            const mat4 MAT(ROT90Z);

            return collisionPtr(new primitiveCollision(
                w, NewtonCreateCylinder(w, diameter * 0.5, height, 0, &MAT[0][0])));
        }

        collisionPtr capsule(const world &w, float diameter, float height)
        {
            const quat ROT90Z(deg(-90), vec3(0, 0, 1));
            const mat4 MAT(ROT90Z);

            return collisionPtr(new primitiveCollision(
                w, NewtonCreateCapsule(w, diameter * 0.5, height, 0, &MAT[0][0])));
        }
    }
}
