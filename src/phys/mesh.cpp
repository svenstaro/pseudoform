 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <vector>

#include <Newton.h>

#include <boost/lexical_cast.hpp>

#include <OgreMeshManager.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>

#include "mesh.hpp"
#include "world.hpp"
#include "types.hpp"
#include "log.hpp"

namespace engine
{
    namespace phys
    {


//        NewtonCollision *mesh::box(const world &w, const vec3 &s)
//        {
//            return NewtonCreateBox(w, s.x, s.y, s.z, NULL);
//        }
//
//        NewtonCollision *mesh::ball(const world &w, const vec3 &s)
//        {
//            // we provide diameter, but newton wants radius
//            return NewtonCreateSphere(w, s.x * 0.5, s.y * 0.5, s.z * 0.5, NULL);
//        }
//
//        NewtonCollision *mesh::cylinder(const world &w, const vec3 &s)
//        {
//            // rotate 90 degrees on Z axis because newton's
//            // height is on X, but our's is on Y
//            mat4 m0(quat(deg(90), vec3(0, 0, 1)));
//            float m1[16];
//            return NewtonCreateCylinder(w, s.x * 0.5, s.y, _matrix(m0, m1));
//        }
//
//        NewtonCollision *mesh::cone(const world &w, const vec3 &s)
//        {
//            // rotate 90 degrees on Z axis because newton's
//            // height is on X, but our's is on Y
//            mat4 m0(quat(deg(90), vec3(0, 0, 1)));
//            float m1[16];
//            return NewtonCreateCone(w, s.x * 0.5, s.y, _matrix(m0, m1));
//        }
//
//        NewtonCollision *mesh::capsule(const world &w, const vec3 &s)
//        {
//            // rotate 90 degrees on Z axis because newton's
//            // height is on X, but our's is on Y
//            mat4 m0(quat(deg(90), vec3(0, 0, 1)));
//            float m1[16];
//            return NewtonCreateCapsule(w, s.x * 0.5, s.y, _matrix(m0, m1));
//        }
//
//        NewtonCollision *mesh::convexFromMesh(const world &w,
//            const std::string &mn, const mat4 &tf)
//        {
//            mesh mesh(mn, tf);
//
//            NewtonCollision *col = mesh.convex(w);
//
//            // convex hulls can be scaled, create a modifier as such
//            //NewtonCollision *mod = NewtonCreateConvexHullModifier(w, col);
//
//            // release old collision, and return the modifier
//            //NewtonReleaseCollision(w, col);
//
//            //return mod;
//            return col;
//        }
//
//        NewtonCollision *mesh::concaveFromMesh(const world &w,
//            const std::string &mn, const mat4 &tf)
//        {
////            mesh mesh(mn, tf);
////            return mesh.concave(w);
//            Ogre::MeshPtr meshPtr;
//            try
//            {
//                meshPtr = Ogre::MeshPtr(Ogre::MeshManager::getSingleton().load(mn,
//                    Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
//            }
//            catch(...)
//            {
//                return NULL;
//            }
//            const Ogre::Mesh &mesh = *meshPtr;
//
//            //if (mesh.getNumSubMeshes() == 1)
//            //    return mesh::convexFromMesh(w, mn, tf);
//
//            std::vector<NewtonCollision*> parts;
//            //parts.reserve(mesh.getNumSubMeshes());
//
//            for (unsigned smi = 0; smi < mesh.getNumSubMeshes(); smi++) {
//                std::vector<vec3> partVerts;
//
//                Ogre::SubMesh *subMesh = mesh.getSubMesh(smi);
//
//                //log("adding submesh " + boost::lexical_cast<string>(smi));
//
//                Ogre::VertexData *vertexData =
//                    (subMesh->useSharedVertices) ?
//                        vertexData = mesh.sharedVertexData : vertexData = subMesh->vertexData;
//
//                Ogre::VertexDeclaration *vertexDecl = vertexData->vertexDeclaration;
//                const Ogre::VertexElement *element = vertexDecl->findElementBySemantic(Ogre::VES_POSITION);
//
//                Ogre::HardwareVertexBufferSharedPtr vertexHVBSP =
//                    vertexData->vertexBufferBinding->getBuffer(element->getSource());
//                unsigned char *vPtr = (unsigned char*)(vertexHVBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
//
//                Ogre::IndexData *indexData = subMesh->indexData;
//                size_t numIndices = indexData->indexCount;
//                size_t numTris = numIndices / 3;
//
//                // get pointer!
//                Ogre::HardwareIndexBufferSharedPtr indexHIBSP = indexData->indexBuffer;
//
//                // 16 or 32 bit indices?
//                bool indicesAre32Bit = (indexHIBSP->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
//                unsigned long *longPtr = NULL;
//                unsigned short *shortPtr = NULL;
//
//                if (indicesAre32Bit)
//                    longPtr =
//                        static_cast<unsigned long*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
//                else
//                    shortPtr =
//                        static_cast<unsigned short*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
//
//                //now loop through the indices, getting polygon info!
//                int iOffset = 0;
//
//                for (size_t i = 0; i < numTris; i++) {
//                    unsigned char *vOffset;
//                    float *vertexPosPtr;
//                    int idx;
//
//                    for (int j = 0; j < 3; j++) {
//                        if (indicesAre32Bit)
//                            idx = longPtr[iOffset + j];
//                        else
//                            idx = shortPtr[iOffset + j];
//
//                        vOffset = vPtr + (idx * vertexHVBSP->getVertexSize());
//                        element->baseVertexPointerToElement(vOffset, &vertexPosPtr);
//
//                        vec3 vert;
//
//                        vert.x = *vertexPosPtr; vertexPosPtr++;
//                        vert.y = *vertexPosPtr; vertexPosPtr++;
//                        vert.z = *vertexPosPtr; vertexPosPtr++;
//
//                        partVerts.push_back(tf * vert);
//                    }
//
//                    iOffset += 3;
//                }
//
//                //unlock the buffers!
//                vertexHVBSP->unlock();
//                indexHIBSP->unlock();
//
//                parts.push_back(NewtonCreateConvexHull(w, partVerts.size(), &partVerts[0].x, 12, 0.01, NULL));
//            }
//
//            NewtonCollision *col = NewtonCreateCompoundCollision(w, parts.size(), &parts[0]);
//            //! should the convex collisions in 'parts' be NewtonReleaseCollision()'d ?!?
//
//            for (size_t i = 0; i < parts.size(); ++i)
//                NewtonReleaseCollision(w, parts[i]);
//
//            return col;
//
//            // convex hulls can be scaled, create a modifier as such
//            //NewtonCollision *mod = NewtonCreateConvexHullModifier(w, col);
//
//            // release old collision, and return the modifier
//            //NewtonReleaseCollision(w, col);
//
//            //return mod;
//        }
//
//        NewtonCollision *mesh::treeFromMesh(const world &w,
//            const std::string &mn, const mat4 &tf,
//            NewtonCollision *outsideTree, int outsideId)
//        {
//            Ogre::MeshPtr meshPtr;
//            try
//            {
//                meshPtr = Ogre::MeshPtr(Ogre::MeshManager::getSingleton().load(mn,
//                    Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
//            }
//            catch(...)
//            {
//                return NULL;
//            }
//            const Ogre::Mesh &mesh = *meshPtr;
//
//            NewtonCollision *tree = NULL;
//            if (!outsideTree) {
//                tree = NewtonCreateTreeCollision(w);
//                NewtonTreeCollisionBeginBuild(tree);
//            } else {
//                tree = outsideTree;
//            }
//
//            // if our tree, use min of int.  otherwise, use passed in id
//            int id = (outsideTree) ? outsideId : std::numeric_limits<int>::min();
//
//            for (unsigned smi = 0; smi < mesh.getNumSubMeshes(); smi++) {
//                Ogre::SubMesh *subMesh = mesh.getSubMesh(smi);
//
//                Ogre::VertexData *vertexData =
//                    (subMesh->useSharedVertices) ?
//                        vertexData = mesh.sharedVertexData : vertexData = subMesh->vertexData;
//
//                Ogre::VertexDeclaration *vertexDecl = vertexData->vertexDeclaration;
//                const Ogre::VertexElement *element = vertexDecl->findElementBySemantic(Ogre::VES_POSITION);
//
//                Ogre::HardwareVertexBufferSharedPtr vertexHVBSP =
//                    vertexData->vertexBufferBinding->getBuffer(element->getSource());
//                unsigned char *vPtr = (unsigned char*)(vertexHVBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
//
//                Ogre::IndexData *indexData = subMesh->indexData;
//                size_t numIndices = indexData->indexCount;
//                size_t numTris = numIndices / 3;
//
//                // get pointer!
//                Ogre::HardwareIndexBufferSharedPtr indexHIBSP = indexData->indexBuffer;
//
//                // 16 or 32 bit indices?
//                bool indicesAre32Bit = (indexHIBSP->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
//                unsigned long *longPtr = NULL;
//                unsigned short *shortPtr = NULL;
//
//                if (indicesAre32Bit)
//                    longPtr =
//                        static_cast<unsigned long*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
//                else
//                    shortPtr =
//                        static_cast<unsigned short*>(indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
//
//                //now loop through the indices, getting polygon info!
//                int iOffset = 0;
//
//                for (size_t i = 0; i < numTris; i++) {
//                    vec3 triVertices[3];
//                    unsigned char *vOffset;
//                    float *vertexPosPtr;
//                    int idx;
//
//                    for (int j = 0; j < 3; j++) {
//                        if (indicesAre32Bit)
//                            idx = longPtr[iOffset + j];
//                        else
//                            idx = shortPtr[iOffset + j];
//
//                        vOffset = vPtr + (idx * vertexHVBSP->getVertexSize());
//                        element->baseVertexPointerToElement(vOffset, &vertexPosPtr);
//
//                        triVertices[j].x = *vertexPosPtr; vertexPosPtr++;
//                        triVertices[j].y = *vertexPosPtr; vertexPosPtr++;
//                        triVertices[j].z = *vertexPosPtr; vertexPosPtr++;
//
//                        triVertices[j] = tf * triVertices[j];
//                    }
//
//                    NewtonTreeCollisionAddFace(tree, 3, &triVertices[0].x, sizeof(float) * 3, id);
//                    if (!(++id)) ++id; // no 0's plz
//
//                    iOffset += 3;
//                }
//
//                //unlock the buffers!
//                vertexHVBSP->unlock();
//                indexHIBSP->unlock();
//            }
//
//            if (!outsideTree)
//                NewtonTreeCollisionEndBuild(tree, 0);
//
//            // return tree if self-made, otherwise the current ID
//            // (see above)
//            return (outsideTree) ? (NewtonCollision*)id : tree;
//        }

        mesh::mesh(const std::string &mn, const mat4 &tf, const world &_world):
        /** Don't know if world() is the best thing to pass but it works for now **/
            _mesh(NewtonMeshCreate(_world))
        {
            Ogre::MeshPtr meshPtr;
            try
            {
                meshPtr = Ogre::MeshPtr(Ogre::MeshManager::getSingleton().load(mn,
                    Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));
            }
            catch(...)
            {
                return;
            }

            const Ogre::Mesh &mesh = *meshPtr;

            NewtonMeshBeginFace(_mesh);

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
                    longPtr = static_cast<unsigned long*>(
                        indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
                else
                    shortPtr = static_cast<unsigned short*>(
                        indexHIBSP->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

                //now loop through the indices, getting polygon info!
                int iOffset = 0;

                for (size_t i = 0; i < numTris; i++) {
                    vec3 triVertices[3];
                    unsigned char *vOffset = NULL;
                    float *vertexPosPtr = NULL;
                    int idx = 0;

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

                        triVertices[j] = tf * triVertices[j];
                    }

                    // _mesh, 3 vertices (triangle), (float = 4 bytes) * 3
                    // index = index of sub mesh (easy to recognize)
                    NewtonMeshAddFace(_mesh, 3, &triVertices[0].x, sizeof(float) * 3, smi);

                    iOffset += 3;
                }

                //unlock the buffers!
                vertexHVBSP->unlock();
                indexHIBSP->unlock();
            }

            NewtonMeshEndFace(_mesh);
        }

        mesh::mesh(const NewtonCollision *col): _mesh(NULL) {
            _mesh = NewtonMeshCreateFromCollision(col);
        }

        mesh::~mesh()
        {
            if (_mesh)
                NewtonMeshDestroy(_mesh);
            _mesh = NULL;
        }

//        NewtonCollision *mesh::convex(const world &w) const
//        {
//            if (!_mesh)
//            {
//                log("mesh::convex() _mesh is NULL");
//                return NULL;
//            }
//
//            const float TOL = 0.02;
//            NewtonCollision *col = NewtonCreateConvexHullFromMesh(w, _mesh, TOL);
//            return col;
//        }
//
//        NewtonCollision *mesh::concave(const world &w) const
//        {
//            assert(false && "not implemented");
//            return NULL;
////            if (!_mesh)
////            {
////                log("mesh::concave() _mesh is NULL");
////                return NULL;
////            }
////
////            const float TOL = 0.01;
////            const size_t MAX_SHAPES = 64;
////            return NewtonCreateCompoundCollisionFromMesh(w, _mesh, TOL, MAX_SHAPES);
//        }
    }
}
