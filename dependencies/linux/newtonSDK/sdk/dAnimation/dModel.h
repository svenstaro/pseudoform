/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/


#ifndef _D_MODEL_H_
#define _D_MODEL_H_

#include <dAnimationStdAfx.h>
#include <dMesh.h>
#include <dList.h>
#include <dMatrix.h>
#include <dMathDefines.h>
#include <dBaseHierarchy.h>

class dBone;
class dModel;
class dAnimationClip;
class dColladaSceneExportContext;


#define XML_HEADER "newton 2.0 file format"


class dLoaderContext
{
	public:
	dLoaderContext() {}
	virtual ~dLoaderContext() {}
	virtual dMesh* CreateMesh ();
	virtual dBone* CreateBone (dBone* parent);
	virtual void LoaderFixup (dModel* model);
};

#define MODEL_COMPONENT_NAME_SIZE 32

template<class T>
class ModelComponent
{
	public:
	char m_name[MODEL_COMPONENT_NAME_SIZE];
	T m_data;
};

template<class T>
class ModelComponentList: public dList < ModelComponent<T> >
{
};

class dModel
{
	public:
	dModel();
	virtual ~dModel();

	void SetMatrix (const dMatrix& matrix);

	void AddMesh(dMesh* mesh);
	void RemoveMesh(dMesh* mesh);

	void AddAnimation(const char *name, dAnimationClip* mesh);
	void RemoveAnimation(dAnimationClip* mesh);

	void AddSkeleton (dBone* rooBone);

	void UpdateMatrixPalette (const dMatrix& parentMatrix, dMatrix* const matrixOut, int maxCount) const;
	void BindMeshToBonesByName () const;

	int GetBoneCount() const;
	dBone* FindBone (int index) const;
	dBone* FindBone (const char* name) const;
	dMesh* FindMesh (const char* name) const;

	dMeshInstance* FindMeshInstance (dMesh* mesh) const;
	dMeshInstance* FindMeshInstance (int boneIndex) const;
	dAnimationClip* FindAnimationClip (const char* name) const;

	void ApplyGlobalScale (dFloat scale);
	void ApplyGlobalTransform (const dMatrix& matrix);
	


	static void GetFileName (const char* pathNamenode, char* name);
	static void GetFilePath (const char* pathNamenode, char* name);
	static int PackVertexArray (dFloat* vertexList, int compareElements, int strideInBytes, int vertexCount, int* indexListOut);

#ifdef LOAD_SAVE_XML
	static int StringToInts (const char* string, int* ints);
	static int StringToFloats (const char* string, dFloat* floats);
	static void IntsToString (char* string, const int* ints, int count);
	static void FloatsToString (char* string, const dFloat* floats, int count);
	virtual void LoadXML (const char* name, dLoaderContext& context); 
	virtual void SaveXML (const char* name, bool exportSkeleton = true, bool exportMesh = true, bool exportAnimations = true); 
#endif
	virtual void SaveCollada (const char *fileName, const dMatrix& globalRotation, dFloat scale) const;
	virtual void LoadCollada (const char* name, dLoaderContext& context, const dMatrix& globalRotation, dFloat scale); 

	dMatrix m_matrix;
	dList<dMeshInstance> m_meshList;
	ModelComponentList<dList<dBone*> > m_skeleton;
//	ModelComponentList<dList<dMeshInstance> > m_meshList;
	ModelComponentList<dAnimationClip*> m_animations;
};


#endif
