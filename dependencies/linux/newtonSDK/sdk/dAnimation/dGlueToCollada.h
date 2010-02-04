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

#ifndef  __SAVE_COLLADA_SCENE_H__
#define  __SAVE_COLLADA_SCENE_H__

#include <dMatrix.h>
#include <Newton.h>

class dMesh;
class dModel;
class dSubMesh;


class dColladaSceneExportContext 
{	

	public: 
	dColladaSceneExportContext ()
	{
	}

	virtual ~dColladaSceneExportContext ()
	{
	}

	virtual dModel* GetModel (NewtonBody* const body) 
	{
		_ASSERTE (0);
		return NULL;
	}

	virtual void DisposeModel (dModel* model)
	{
	}
};


void ExportColladaScene (const char* name, const NewtonWorld* world, dColladaSceneExportContext* context, const dMatrix& globalRotation, dFloat scale);



#endif