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

#ifndef __OGL_MODEL_H__
#define __OGL_MODEL_H__

#include <dModel.h>

#define MAX_PALETTE_MATRIX  512

class OGLLoaderContext: public dLoaderContext
{
	public:
	OGLLoaderContext();
	~OGLLoaderContext();
	virtual dMesh* CreateMesh (int type);
	virtual void LoaderFixup (dModel* model);
};


class OGLModel: public dModel
{
public:
	OGLModel(void);
	virtual ~OGLModel(void);

	virtual void Render();
	void* m_node;

	static dMatrix m_matrixPalete[MAX_PALETTE_MATRIX];
};

#endif
